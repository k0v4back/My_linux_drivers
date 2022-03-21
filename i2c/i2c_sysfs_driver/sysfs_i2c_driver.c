#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/i2c.h>

static int sysfs_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int sysfs_i2c_remove(struct i2c_client *client);
static int sysfs_i2c_write(unsigned char *buf, unsigned int len, struct i2c_client *client);
static int sysfs_i2c_read(unsigned char *buf, unsigned int len, struct i2c_client *client);

/* Client data from DT */
static struct platform_device_data {
        unsigned size;
        unsigned reg;
        const char *name;
};

/* Device private data structure */
static struct device_private_data {
        struct platform_device_data     pdata;
        struct i2c_client               *client;
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

/* Check device tree and get data*/
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
        int err;
        
        dev_info(dev, "I2C device is detected\n");

        /* Check device tree and get data*/
        pdata = get_platform_data_dt(client);
        if(IS_ERR(pdata))
                return PTR_ERR(pdata);

        /* Allocate memory for device private data */
        dev_data = devm_kzalloc(dev, sizeof(*dev_data), GFP_KERNEL);
        if(!dev_data){
                dev_info(dev, "Cannot allocate memory for device private data\n");
                err = -ENOMEM;
        }

        /* Save i2c client */
        dev_data->client = client;

        /* Save the device private data pointer in platform device structure */
        i2c_set_clientdata(client, dev_data);

        /* Save data from DT */
        dev_data->pdata.size = pdata->size;
        dev_data->pdata.name= pdata->name;
        dev_info(dev, "Device size = %d\n", dev_data->pdata.size);
        dev_info(dev, "Device name = %s\n", dev_data->pdata.name);

        unsigned char buf[5] = "k0v4";
        sysfs_i2c_write(buf, 5, client);

        dev_info(dev, "Probe function was successful\n");

        return 0;
}

static int sysfs_i2c_remove(struct i2c_client *client)
{
        struct device *dev = &client->dev;

        dev_info(dev, "Remove Function is invoked...\n"); 

        return 0;
}


/* File operation functions */ 

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
        i2c_del_driver(&sysfs_i2c_driver);
        pr_info("Module unloaded\n");
}

module_init(sysfs_i2c_init);
module_exit(sysfs_i2c_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim");
