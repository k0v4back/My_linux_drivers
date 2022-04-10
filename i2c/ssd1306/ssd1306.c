#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/sysfs.h>

static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int ssd1306_remove(struct i2c_client *client);
static int ssd1306_write(unsigned char *buf, unsigned int len, struct i2c_client *client);
static int ssd1306_read(unsigned char *buf, unsigned int len, struct i2c_client *client);
static ssize_t show_name(struct device *dev, struct device_attribute *attr, char *buf);
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

static struct of_device_id ssd1306_of_match[] = {
        {
                .compatible = "ssd1306",
                .data = 0
        },
        { }
};

static const struct i2c_device_id ssd1306_id[] = {
        { "ssd1306", 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, ssd1306_id);

static struct i2c_driver ssd1306 = {
        .driver = {
                .name               = "ssd1306",
                .owner              = THIS_MODULE,
                .of_match_table     = of_match_ptr(ssd1306_of_match),
        },

        .probe          = ssd1306_probe,
        .remove         = ssd1306_remove,
        .id_table       = ssd1306_id,
};

/*
 *  Variables of struct device_attribute
 */
static DEVICE_ATTR(name, S_IRUGO, show_name, NULL);

static struct attribute *ssd1306_attrs[] = 
{
        &dev_attr_name.attr,
        NULL
};

static struct attribute_group ssd1306_attrs_group =
{
        .attrs = ssd1306_attrs
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

        ret = sysfs_create_group(&dev->kobj, &ssd1306_attrs_group);

        return ret;
}

/* Check device tree and get data*/
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

        if(of_property_read_string(dev_node, "ssd1306,name", &pdata->name)){
                dev_info(dev, "Missing name property \n");
                return ERR_PTR(-EINVAL);
        }

        return pdata;
}

static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct device_private_data *dev_data;
        struct device *dev = &client->dev;
        struct platform_device_data *pdata;
        int ret;
        u32 temp;
        
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

        dev = root_device_register("ssd1306");
        dev_set_drvdata(dev, dev_data->client);
        ret = platform_driver_sysfs_create_files(dev);
        if(ret){
                pr_info("sysfs_create_group failure.\n");
        }

        unsigned char buf[5] = "k0v4";
        ssd1306_write(buf, 5, dev_data->client);

        dev_info(dev, "Probe function was successful\n");

        return 0;
}

static int ssd1306_remove(struct i2c_client *client)
{
        struct device *dev = &client->dev;

        dev_info(dev, "Remove Function is invoked...\n"); 

        return 0;
}


/* File operation functions */ 

static int ssd1306_write(unsigned char *buf, unsigned int len, struct i2c_client *client)
{
    int ret = i2c_master_send(client, buf, len);

    return ret;
}

static int ssd1306_read(unsigned char *buf, unsigned int len, struct i2c_client *client)
{
    int ret = i2c_master_recv(client, buf, len);

    return ret;
}


static int __init ssd1306_init(void)
{
        int ret;
        
        ret = i2c_add_driver(&ssd1306);
        if(ret != 0){
                pr_err("%s:driver registration failed i2c-slave, error=%d\n", __func__, ret);
                i2c_del_driver(&ssd1306);
                return ret;
        }

        pr_info("Platform ssd1306 driver loaded\n");

        return 0;
}

static void __exit ssd1306_cleanup(void)
{
        i2c_del_driver(&ssd1306);
        pr_info("Module unloaded\n");
}

module_init(ssd1306_init);
module_exit(ssd1306_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("ssd1306 display");
