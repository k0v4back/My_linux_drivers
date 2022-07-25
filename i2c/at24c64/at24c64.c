/*
 * at24c64.c - Linux kernel module to store 64K(64Kb = 8KiB) of data.
 * Get and write data.
 */

#include <linux/delay.h>
#include <linux/hwmon.h>
#include <linux/i2c.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>


#define AT24C64_WRITE           0b10100000      /* Write to eeprom */
#define AT24C64_READ            0b10100001      /* Read from eeprom */

#define ARRAY_ELEMENTS(arr)     (sizeof(arr) / sizeof(arr[0]))

static int at24c64_probe(struct i2c_client *client,
        const struct i2c_device_id *id);
static int at24c64_remove(struct i2c_client *client);
static void at24c64_write_byte(struct i2c_client *client,
        const u16 addr, u8 * const byte);
static void at24c64_read_byte(struct i2c_client *client,
        const u16 addr, u8 * const byte);
static void at24c64_write_page(struct i2c_client *client,
        const u16 addr, u8 * const arr, u8 count);
static void at24c64_read_page(struct i2c_client *client,
        const u16 addr, u8 * const arr, u8 count);
static int at24c64_open(struct inode *pinode, struct file *pfile); 
static ssize_t at24c64_write(struct file *pfile, const char __user *ubuff,
        size_t count, loff_t *fpos);
static ssize_t at24c64_read(struct file *pfile, char __user *ubuff, size_t count,
        loff_t *fpos); 
static int at24c64_release(struct inode *pinode, struct file *pfile);

u8 write_byte = 0xB;
u8 get_byte = 0;
u8 write_page[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
u8 read_page[10];

/* Data about at24c64 eeprom */
static struct at24c64_chip_data {
        const char *name;
};

/* Device private data structure */
static struct device_private_data {
        struct at24c64_chip_data *pdata;
};

/* Driver information */
static struct driver_private_data {
        dev_t base_dev_num;
        struct cdev *cdev;
        struct device *device;
        struct class *class;
        struct i2c_client *client;
};

static struct driver_private_data driver_data;

static struct file_operations at24c64_fops = {
        .open = at24c64_open,
        .write = at24c64_write,
        .read = at24c64_read,
        .release = at24c64_release,
        .owner = THIS_MODULE
};

static struct of_device_id at24c64_of_match[] = {
        {
                .compatible = "at24c64",
                .data = 0
        },
        { }
};

static const struct i2c_device_id at24c64_id[] = {
        { 
                .name = "at24c64",
                .driver_data = 0
        },
        { }
};
MODULE_DEVICE_TABLE(i2c, at24c64_id);


static struct i2c_driver at24c64 = {
        .driver = {
                .name               = "at24c64",
                .owner              = THIS_MODULE,
                .of_match_table     = of_match_ptr(at24c64_of_match),
        },

        .probe          = at24c64_probe,
        .remove         = at24c64_remove,
        .id_table       = at24c64_id,
};


/*
 * Functions for talking with eeprom
 */
static void at24c64_write_byte(struct i2c_client *client,
        const u16 addr, u8 * const byte)
{
        int ret = 0;
        const u8 command[] = {addr >> 8, addr, *byte};

        ret = i2c_master_send(client, command, ARRAY_ELEMENTS(command));
        if (ret != ARRAY_ELEMENTS(command)) 
                dev_info(&client->dev, "Write %d byte\n", ret);
}

static void at24c64_read_byte(struct i2c_client *client,
        const u16 addr, u8 * const byte)
{
        int ret = 0;
        const u8 command[] = {addr >> 8, addr};

        ret = i2c_master_send(client, command, ARRAY_ELEMENTS(command));
        if (ret != ARRAY_ELEMENTS(command)) 
                dev_info(&client->dev, "Write %d byte\n", ret);

        usleep_range(350000, 350000 + 100000);

        ret = i2c_master_recv(client, byte, 1);
        if (ret != 1)
                dev_info(&client->dev, "Recive %d byte\n", ret);
}

static void at24c64_write_page(struct i2c_client *client,
        const u16 addr, u8 * const arr, u8 count)
{
        int ret = 0;
        int i = 0;
        u8 *command;

        command = devm_kzalloc(&client->dev, (count + 2)*sizeof(u8),
                GFP_KERNEL);
        if (!command)
                dev_info(&client->dev, "Cannot allocate memory for page");

        command[0] = addr >> 8;
        command[1] = addr;
        for(i = 0; i < count; i++)
                command[i+2] = arr[i];

        ret = i2c_master_send(client, command, (count + 2));
        if (ret != (count + 2)) 
                dev_info(&client->dev, "Write %d byte\n", ret);
}

static void at24c64_read_page(struct i2c_client *client,
        const u16 addr, u8 * const arr, u8 count)
{
        int ret = 0;
        const u8 command[] = {addr >> 8, addr};

        ret = i2c_master_send(client, command, ARRAY_ELEMENTS(command));
        if (ret != ARRAY_ELEMENTS(command)) 
                dev_info(&client->dev, "Write %d byte\n", ret);

        usleep_range(350000, 350000 + 100000);

        ret = i2c_master_recv(client, arr, count);
        if (ret != count)
                dev_info(&client->dev, "Recive %d byte\n", ret);
        dev_info(&client->dev, "Recive %d byte\n", ret);
}


/*
 * Check device tree and get data
 */
static struct at24c64_chip_data * get_platform_data_dt(struct i2c_client *client)
{
        struct device *dev = &client->dev;
        struct device_node *dev_node = dev->of_node;
        struct at24c64_chip_data *pdata;

        if (!dev_node)
                return NULL;

        /* Allocate memory for pdata */
        pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
        if (!pdata) {
                dev_info(dev, "Cannot allocate memory for at24c64_chip_data");
                return ERR_PTR(-ENOMEM);
        }

        /* 
         * Extract propertes of the device node using dev_node
         * and put into struct platform_device_data 
         */
        if (of_property_read_string(dev_node, "at24c64,name", &pdata->name)) {
                dev_info(dev, "Missing name property \n");
                return ERR_PTR(-EINVAL);
        }

        return pdata;
}

static int at24c64_probe(struct i2c_client *client,
        const struct i2c_device_id *id)
{
        struct device_private_data *dev_data;
        struct device *dev = &client->dev;
        int ret;


        /* Allocate memory for device */
        dev_data = devm_kzalloc(&client->dev, sizeof(*dev_data), GFP_KERNEL);
        if (dev_data == NULL) {
                dev_info(dev, "Cannot allocate memory for device_private_data struct\n");
                ret = -ENOMEM;
        }

        /* Check device tree and get data*/
        dev_data->pdata = get_platform_data_dt(client);

        /* Save i2c client */
        driver_data.client = client;

        /* Save the device private data pointer to device structure */
        i2c_set_clientdata(client, dev_data);

        dev_info(dev, "Device name = %s\n", dev_data->pdata->name);

        dev = root_device_register("at24c64");

        /*
        at24c64_write_byte(client, 0x00F8, &write_byte);  
        usleep_range(350000, 350000 + 100000);
        at24c64_read_byte(client, 0x00F8, &get_byte);
        dev_info(dev, "Read data by 0x00F8 = %x\n", get_byte);
        */

        at24c64_write_page(client, 0xA, write_page, ARRAY_ELEMENTS(write_page));
        usleep_range(350000, 350000 + 100000);
        at24c64_read_page(client, 0xA, read_page, ARRAY_ELEMENTS(read_page));


        dev_info(dev, "Probe function was successful\n");

        return 0;
}


/* File operation methods */
static int at24c64_open(struct inode *pinode, struct file *pfile) 
{
        return 0;
}

static ssize_t at24c64_write(struct file *pfile, const char __user *ubuff,
        size_t count, loff_t *fpos)
{
        return -ENOMEM;
}

static ssize_t at24c64_read(struct file *pfile, char __user *ubuff, size_t count,
        loff_t *fpos) 
{
        return 0;
}

static int at24c64_release(struct inode *pinode, struct file *pfile)
{
        pr_info("Close was successful\n");
        return 0;
}

static int at24c64_remove(struct i2c_client *client)
{
        struct device *dev = &client->dev;

        i2c_set_clientdata(client, NULL);

        dev_info(dev, "Remove Function is invoked...\n");

        return 0;
}

static int __init at24c64_driver_init(void)
{
        int ret;

        /* Allocate device number dynamically */
        ret = alloc_chrdev_region(&driver_data.base_dev_num, 0, 1, "at24c64_eeprom");
        if (ret != 0) {
                pr_err("Chrdev allocation failed\n");
                return ret;
        }

        /* Register i2c driver */
        ret = i2c_add_driver(&at24c64);
        if (ret != 0) {
                pr_err("%s:driver registration failed \
                        i2c-slave, error=%d\n", __func__, ret);
                i2c_del_driver(&at24c64);
                return ret;
        }

        /* Init cdev */
        cdev_init(driver_data.cdev, &at24c64_fops);

        /* Add cdev, cdev structure register with VFS */
        ret = cdev_add(driver_data.cdev, driver_data.base_dev_num, 1);
        if (ret < 0) 
                pr_err("Cdev add was fail\n");

        /* Create device class */
        driver_data.class = class_create(THIS_MODULE, "at24c64_eeprom_class");
        if (IS_ERR(&driver_data.class)) {
                pr_err("Class creation failed\n");
                return 0;
        }

        /* Create device file for at24c64 eeprom */
        driver_data.device = device_create(driver_data.class, NULL, driver_data.base_dev_num,
                NULL, "at24c64_eeprom-%d", 1);
        if (IS_ERR(driver_data.device)) {
                pr_info("Device creation was fail\n");
                return 0;
        }

        pr_info("Platform at24c64 driver loaded\n");

        return 0;
}

static void __exit at24c64_driver_cleanup(void)
{
        /* Unregister i2c driver */
        i2c_del_driver(&at24c64);

        /* Class destroy */
        class_destroy(driver_data.class);

        /* Unregister device */
        unregister_chrdev_region(driver_data.base_dev_num, 1);

        pr_info("Module unloaded\n");
}

module_init(at24c64_driver_init);
module_exit(at24c64_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("AT24C64 eeprom");
