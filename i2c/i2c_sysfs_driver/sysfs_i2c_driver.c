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

static int sysfs_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int sysfs_i2c_remove(struct i2c_client *client);
static int sysfs_i2c_write(unsigned char *buf, unsigned int len, struct i2c_client *client);
static int sysfs_i2c_read(unsigned char *buf, unsigned int len, struct i2c_client *client);
static ssize_t show_size(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t show_name(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t store_send_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static int platform_driver_sysfs_create_files(struct device *pcd_dev);

/* Client data from DT */
static struct platform_device_data {
        unsigned size;
        unsigned reg;
        const char *name;
};

/* Device private data structure */
static struct device_private_data {
        struct platform_device_data pdata;
        struct i2c_client *client;
        char *buffer;
        dev_t dev_num;
        struct cdev cdev;
};

static enum i2c_device_names {
        sysfs_i2c_driver_first,
};

static struct of_device_id sysfs_i2c_of_match[] = {
        {
                .compatible = "sysfs_i2c_driver",
                .data = (void*)sysfs_i2c_driver_first
        },
        { }
};

static const struct i2c_device_id sysfs_i2c_id[] = {
        { "sysfs_i2c_driver", sysfs_i2c_driver_first },
        { }
};
MODULE_DEVICE_TABLE(i2c, sysfs_i2c_id);

static struct i2c_driver sysfs_i2c_driver = {
        .driver = {
                .name               = "sysfs_i2c_driver",
                .owner              = THIS_MODULE,
                .of_match_table     = of_match_ptr(sysfs_i2c_of_match),
        },

        .probe          = sysfs_i2c_probe,
        .remove         = sysfs_i2c_remove,
        .id_table       = sysfs_i2c_id,
};

/*
 *  3 variables of struct device_attribute
 */
static DEVICE_ATTR(size, S_IRUGO, show_size, NULL);
static DEVICE_ATTR(name, S_IRUGO, show_name, NULL);
static DEVICE_ATTR(send_data, S_IWUSR, NULL, store_send_data);

struct attribute *sysfs_i2c_attrs[] = 
{
        &dev_attr_size.attr,
        &dev_attr_name.attr,
        &dev_attr_send_data.attr,
        NULL
};

struct attribute_group sysfs_i2c_attrs_group =
{
        .attrs = sysfs_i2c_attrs
};


/*
 * Device attribute functions
 */
static ssize_t show_size(struct device *dev, struct device_attribute *attr, char *buf)
{
        pr_info("It is show_size\n");

        return 0;
}

static ssize_t show_name(struct device *dev, struct device_attribute *attr, char *buf)
{
        pr_info("It is show_name\n");
        return 0;
}

static ssize_t store_send_data(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        pr_info("count=%d, buf=%s", count, buf);

        /*
        struct device_private_data *dev_data = dev_get_drvdata(dev);
        sysfs_i2c_write(buf, count, dev_data->client);
        */

        return count;
}

static int platform_driver_sysfs_create_files(struct device *dev)
{
        int ret;
        
        ret = sysfs_create_group(&dev->kobj, &sysfs_i2c_attrs_group);
        
        return ret;
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
        pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
        if(!pdata){
                dev_info(dev, "Cannot allocate memory \n");
                return ERR_PTR(-ENOMEM);
        }

        /* Extract propertes of the device node using dev_node 
         * and put into struct platform_device_data */

        if(of_property_read_string(dev_node, "sysfs,name", &pdata->name)){
                dev_info(dev, "Missing name property \n");
                return ERR_PTR(-EINVAL);
        }

        if(of_property_read_u32(dev_node, "sysfs,size", &pdata->size)){
                dev_info(dev, "Missing size property \n");
                return ERR_PTR(-EINVAL);
        }
        
        return pdata;
}

static int sysfs_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct device_private_data *dev_data;
        struct device *dev = &client->dev;
        struct platform_device_data *pdata;
        int ret;
        
        dev_info(dev, "I2C device is detected\n");

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
        dev_data->pdata.size = pdata->size;
        dev_data->pdata.name= pdata->name;
        dev_info(dev, "Device size = %d\n", dev_data->pdata.size);
        dev_info(dev, "Device name = %s\n", dev_data->pdata.name);

        /* Save the device private data pointer in platform device structure */
        i2c_set_clientdata(client, dev_data);

        dev = root_device_register("my_sensor");
        ret = platform_driver_sysfs_create_files(dev);
        if(ret){
                pr_info("sysfs_create_group failure.\n");
        }

        dev_info(dev, "Probe function was successful\n");

        return 0;
}

static int sysfs_i2c_remove(struct i2c_client *client)
{
        struct device *dev = &client->dev;

        dev_info(dev, "Remove Function is invoked...\n"); 

        return 0;
}

/*
 *File operation functions 
 */ 
static int sysfs_i2c_write(unsigned char *buf, unsigned int len, struct i2c_client *client)
{
    int ret = i2c_master_send(client, buf, len);

    return ret;
}

static int sysfs_i2c_read(unsigned char *buf, unsigned int len, struct i2c_client *client)
{
    int ret = i2c_master_recv(client, buf, len);

    return ret;
}


static int __init sysfs_i2c_init(void)
{
        int ret;

        ret = i2c_add_driver(&sysfs_i2c_driver);
        if(ret != 0){
                pr_err("%s:driver registration failed i2c-slave, error=%d\n", __func__, ret);
                i2c_del_driver(&sysfs_i2c_driver);
                return ret;
        }

        pr_info("Platform I2C driver loaded\n");

        return 0;
}

static void __exit sysfs_i2c_cleanup(void)
{
        /* Unregister i2c driver */
        i2c_del_driver(&sysfs_i2c_driver);

        pr_info("Module unloaded\n");
}

module_init(sysfs_i2c_init);
module_exit(sysfs_i2c_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim");
