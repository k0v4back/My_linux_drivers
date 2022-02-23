#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h> 
#include <linux/i2c.h> 
#include <linux/fs.h>
#include <linux/err.h> 
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/of_device.h>

#define MAX_DEVICES 3

enum i2c_device_names {
        test_i2c_driver,
};

/* Device private data structure */
struct device_private_data {
        struct platform_device_data     pdata;
        struct i2c_client               *client;
        dev_t                           dev_num;
        char                            *buffer;
        struct cdev                     cdev;
};

/* Driver private data structure */
struct driver_private_data
{
        int             total_devices;
        dev_t           device_number_base;
        struct class    *class_chardriver;
        struct device   *device_chardriver;
};

struct driver_private_data driver_data;

/* Client data from DT */
struct platform_device_data
{
        unsigned size;
};


static const struct i2c_device_id test_i2c_id[] = {
        { "test_i2c_driver", test_i2c_driver },
        { },
};
MODULE_DEVICE_TABLE(i2c, test_i2c_id);

static const struct of_device_id test_i2c_of_match[] = { 
        {
                .compatible = "org,test_i2c_driver",
                .data = (void *)test_i2c_driver
        },
        { },
};

static struct i2c_driver test_i2c_driver = {
        .driver = {
                .name               = "test_i2c_driver",
                .owner              = THIS_MODULE,
                .of_match_table     = test_i2c_of_match,
        },

        .probe          = test_i2c_probe,
        .remove         = test_i2c_remove,
        .id_table       = test_i2c_id,
};

static int test_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        int ret;
        struct device_private_data *dev_data;
        struct device *dev = &client->dev;
        struct platform_device_data *pdata;
        
        dev_info(dev, "I2C device is detected\n");

        /* Check device tree and get data*/
        pdata = char_driver_get_platform_dt(dev);
        if(IS_ERR(pdata))
                return PTR_ERR(pdata);

        /* Allocate memory for device private data */
        dev_data = dev_kzalloc(dev, sizeof(*dev_data), GFP_KERNEL);
        if(!dev_data){
                dev_info(dev, "Cannot allocate memory for device private data\n");
                ret = -ENOMEM;
        }

        /* Save i2c client */
        dev_data->client = client;

        /* Save the device private data pointer in platform device structure */
        i2c_set_clientdata(client, dev_data);

        /* Save data from DT */
        dev_data->pdata.size = pdata->size;
        dev_info(dev, "Device size = %d\n", dev_data->pdata.size);



        return 0;
}

static int test_i2c_remove(struct i2c_client *client)
{
        return 0;
}

static int __init test_i2c_init(void)
{
        int ret;

        /* Dynamically allocate a device number for all devices  */
        ret = alloc_chrdev_region(&driver_data.device_number_base, 0, MAX_DEVICES, "i2c devices"); 
        if(ret < 0){
                pr_err("Alloc chrdev faild\n");
                return ret;
        }

        /* Create device class under /sys/class  */
        driver_data.class_chardriver = class_create(THIS_MODULE, "driver_data_class");
        if(IS_ERR(driver_data.class_chardriver)){
                pr_err("Class creation failed\n");
                ret = PTR_ERR(driver_data.class_chardriver);
                unregister_chrdev_region(driver_data.device_number_base, MAX_DEVICES);
                i2c_del_driver(&test_i2c_driver);
                return ret; 
        }
        
        ret = i2c_add_driver(&test_i2c_driver);
        if(ret != 0){
                pr_err("%s:driver registration failed i2c-slave, error=%d\n", __func__, ret);
                unregister_chrdev_region(driver_data.device_number_base, MAX_DEVICES);
                i2c_del_driver(&test_i2c_driver);
                return ret;
        }

        pr_info("Platform I2C driver loaded\n");

        return 0
}

static void __exit test_i2c_cleanup(void)
{
        unregister_chrdev_region(driver_data.device_number_base, MAX_DEVICES);
        i2c_del_driver(&test_i2c_driver);
}

module_init(test_i2c_init);
module_exit(test_i2c_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim");
