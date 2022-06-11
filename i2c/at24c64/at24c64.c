/*
 * at24c64.c - Linux kernel modules for hardware monitoring.
 * Get temperature and humidity.
 */

#include <linux/delay.h>
#include <linux/hwmon.h>
#include <linux/i2c.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/sysfs.h>

static int at24c64_probe(struct i2c_client *client,
        const struct i2c_device_id *id);
static int at24c64_remove(struct i2c_client *client);
static ssize_t show_name(struct device *dev,
        struct device_attribute *attr, char *buf);
static int platform_driver_sysfs_create_files(struct device *dev);
static void platform_driver_sysfs_remove_files(struct device *dev);

/* Data about at24c64 eeprom */
static struct at24c64_chip_data {
        const char *name;
        u8 flags;
        u32 byte_len;
};

/* Device private data structure */
static struct device_private_data {
        struct at24c64_chip_data *pdata;
        struct i2c_client *client;
};


static struct of_device_id at24c64_of_match[] = {
        {
                .compatible = "at24c64",
                .data = 0
        },
        { }
};

static const struct i2c_device_id at24c64_id[] = {
        { "at24c64", 0 },
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
 *  Variables of struct device_attribute
 */
static DEVICE_ATTR(name, S_IRUGO, show_name, NULL);

static struct attribute *at24c64_attrs[] = {
        &dev_attr_name.attr,
        NULL
};

static struct attribute_group at24c64_attrs_group = {
        .attrs = at24c64_attrs
};


/*
 * Device attribute functions
 */
static ssize_t show_name(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        pr_info("It is show_name\n");

        return 0;
}


/* Add and remove sysfs group (files) */
static int platform_driver_sysfs_create_files(struct device *dev)
{
        return sysfs_create_group(&dev->kobj, &at24c64_attrs_group);
}

static void platform_driver_sysfs_remove_files(struct device *dev)
{
        sysfs_remove_group(&dev->kobj, &at24c64_attrs_group);
        kobject_del(&dev->kobj);
}


/*
 * Check device tree and get data
 */
static struct at24c64_chip_data * get_platform_data_dt(struct i2c_client *client)
{
        struct device *dev = &client->dev;
        struct device_node *dev_node = dev->of_node;
        struct at24c64_chip_data *pdata;

        if(!dev_node)
                return NULL;

        /* Allocate memory for pdata */
        pdata = devm_kzalloc(dev, sizeof(struct at24c64_chip_data),
                GFP_KERNEL);
        if(!pdata){
                dev_info(dev, "Cannot allocate memory for at24c64_chip_data");
                return ERR_PTR(-ENOMEM);
        }

        /* 
         * Extract propertes of the device node using dev_node
         * and put into struct platform_device_data 
         */
        if(of_property_read_string(dev_node, "at24c64,name", &pdata->name)){
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
        dev_data = devm_kzalloc(&client->dev,
                sizeof(struct device_private_data), GFP_KERNEL);
        if(dev_data == NULL){
                dev_info(dev, "Cannot allocate memory for device_private_data struct\n");
                ret = -ENOMEM;
        }

        /* Check device tree and get data*/
        dev_data->pdata = get_platform_data_dt(client);

        /* Save i2c client */
        dev_data->client = client;

        dev_info(dev, "Device name = %s\n", dev_data->pdata->name);

        /* Save the device private data pointer in platform device structure */
        i2c_set_clientdata(client, dev_data);

        dev = root_device_register("at24c64");
        dev_set_drvdata(dev, dev_data->client);
        ret = platform_driver_sysfs_create_files(dev);
        if(ret){
                pr_info("sysfs_create_group failure.\n");
        }

        dev_info(dev, "Probe function was successful\n");

        return 0;
}

static int at24c64_remove(struct i2c_client *client)
{
        struct device *dev = &client->dev;

        /* Delete existing sysfs group */
        platform_driver_sysfs_remove_files(dev);

        i2c_set_clientdata(client, NULL);

        dev_info(dev, "Remove Function is invoked...\n");

        return 0;
}

static int __init at24c64_driver_init(void)
{
        int ret;

        ret = i2c_add_driver(&at24c64);
        if(ret != 0){
                pr_err("%s:driver registration failed \
                        i2c-slave, error=%d\n", __func__, ret);
                i2c_del_driver(&at24c64);
                return ret;
        }

        pr_info("Platform at24c64 driver loaded\n");

        return 0;
}

static void __exit at24c64_driver_cleanup(void)
{
        /* Unregister i2c driver */
        i2c_del_driver(&at24c64);

        pr_info("Module unloaded\n");
}

module_init(at24c64_driver_init);
module_exit(at24c64_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("AT24C64 eeprom");
