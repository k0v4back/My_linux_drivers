#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/i2c.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include <linux/platform_device.h>
#include<linux/slab.h>
#include<linux/mod_devicetable.h>
#include<linux/of.h>
#include<linux/of_device.h>
#include <linux/delay.h>

#define AHT10_COMAND_INIT 0b11100001

static int aht10_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int aht10_remove(struct i2c_client *client);
static int aht10_write(unsigned char *buf, unsigned int len, struct i2c_client *client);
static int aht10_read(unsigned char *buf, unsigned int len, struct i2c_client *client);
static ssize_t show_size(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t show_name(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t store_send_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static int platform_driver_sysfs_create_files(struct device *pcd_dev);

/* Client data from DT */
static struct platform_device_data {
        const char *name;
};

/* Device private data structure */
static struct device_private_data {
        struct platform_device_data pdata;
        struct i2c_client *client;
};

static struct of_device_id aht10_of_match[] = {
        {
                .compatible = "aht10",
                .data = 0
        },
        { }
};

enum aht10_type {
        aht10_t,
};

static const struct i2c_device_id aht10_id[] = {
        { "aht10", aht10_t },
        { }
};
MODULE_DEVICE_TABLE(i2c, aht10_id);

static struct i2c_driver aht10 = {
        .driver = {
                .name               = "aht10",
                .owner              = THIS_MODULE,
                .of_match_table     = of_match_ptr(aht10_of_match),
        },

        .probe          = aht10_probe,
        .remove         = aht10_remove,
        .id_table       = aht10_id,
};

/*
 *  Variables of struct device_attribute
 */
static DEVICE_ATTR(name, S_IRUGO, show_name, NULL);

static struct attribute *aht10_attrs[] = 
{
        &dev_attr_name.attr,
        NULL
};

static struct attribute_group aht10_attrs_group =
{
        .attrs = aht10_attrs
};


/*
 * Device attribute functions
 */
static ssize_t show_name(struct device *dev, struct device_attribute *attr, char *buf)
{
        pr_info("It is show_name\n");
        return 0;
}

static int platform_driver_sysfs_create_files(struct device *dev)
{
        int ret;
        ret = sysfs_create_group(&dev->kobj, &aht10_attrs_group);
        return ret;
}


static int aht10_sensor_init(struct device_private_data *dev_data)
{
        int ret;
        u8 status;

        ret = i2c_master_send(dev_data->client, AHT10_COMAND_INIT, 1);
        if(ret != 0)
                return ret;

        usleep_range(350000, 350000 + 100000);

        ret = i2c_master_recv(dev_data->client, &status, 1);
        if(ret != 1)
                return -ENODATA;

        return status;
}


/*
 * Check device tree and get data
 */
struct platform_device_data * get_platform_data_dt(struct i2c_client *client)
{
        struct device *dev = &client->dev;
        struct device_node *dev_node = dev->of_node;
        struct platform_device_data *pdata;

        if(!dev_node)
                return NULL;
        
        /* Allocate memory for pdata */
        pdata = devm_kzalloc(dev, sizeof(struct platform_device_data), GFP_KERNEL);
        if(!pdata){
                dev_info(dev, "Cannot allocate memory for platform_device_data\n");
                return ERR_PTR(-ENOMEM);
        }

        /* Extract propertes of the device node using dev_node 
         * and put into struct platform_device_data */

        if(of_property_read_string(dev_node, "aht10,name", &pdata->name)){
                dev_info(dev, "Missing name property \n");
                return ERR_PTR(-EINVAL);
        }

        return pdata;
}

static int aht10_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct device_private_data *dev_data;
        struct device *dev = &client->dev;
        struct platform_device_data *pdata;
        int ret;
        
        /* Check device tree and get data*/
        pdata = get_platform_data_dt(client);
        if(IS_ERR(pdata))
                return PTR_ERR(pdata);

        /* Allocate memory for device */
        dev_data = devm_kzalloc(&client->dev, sizeof(struct device_private_data), GFP_KERNEL);
        if(dev_data == NULL){
                dev_info(dev, "Cannot allocate memory for device_private_data struct\n");
                ret = -ENOMEM;
        }

        /* Save i2c client */
        dev_data->client = client;

        /* Save data from DT */
        dev_data->pdata.name= pdata->name;
        dev_info(dev, "Device name = %s\n", dev_data->pdata.name);

        /* Save the device private data pointer in platform device structure */
        i2c_set_clientdata(client, dev_data);

        dev = root_device_register("aht10");
        ret = platform_driver_sysfs_create_files(dev);
        if(ret){
                pr_info("sysfs_create_group failure.\n");
        }

        ret = aht10_sensor_init(dev_data);
        dev_info(dev, "STATUS = %d\n", ret);

        dev_info(dev, "Probe function was successful\n");

        return 0;
}

static int aht10_remove(struct i2c_client *client)
{
        struct device *dev = &client->dev;

        dev_info(dev, "Remove Function is invoked...\n"); 

        return 0;
}

/*
 *File operation functions 
 */ 
static int aht10_write(unsigned char *buf, unsigned int len, struct i2c_client *client)
{
        int ret = i2c_master_send(client, buf, len);

        return ret;
}

static int aht10_read(unsigned char *buf, unsigned int len, struct i2c_client *client)
{
        int ret = i2c_master_recv(client, buf, len);

        return ret;
}



static int __init aht10_driver_init(void)
{
        int ret;

        ret = i2c_add_driver(&aht10);
        if(ret != 0){
                pr_err("%s:driver registration failed i2c-slave, error=%d\n", __func__, ret);
                i2c_del_driver(&aht10);
                return ret;
        }

        pr_info("Platform I2C driver loaded\n");

        return 0;
}

static void __exit aht10_driver_cleanup(void)
{
        /* Unregister i2c driver */
        i2c_del_driver(&aht10);

        pr_info("Module unloaded\n");
}

module_init(aht10_driver_init);
module_exit(aht10_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("AHT10 sensor");