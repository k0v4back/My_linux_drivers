#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/mod_devicetable.h>
#include <linux/spi/spi.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/uaccess.h>

static int test_spi_probe(struct spi_device *device);
static int test_spi_remove(struct spi_device *device);
static int test_spi_write(unsigned char *buf, unsigned int len, struct spi_device *device);
static int test_spi_read(unsigned char *buf, unsigned int len, struct spi_device *device);

/* Client data from DT */
struct platform_device_data {
        unsigned size;
        unsigned reg;
        const char *name;
};

/* Device private data structure */
struct device_private_data {
        struct platform_device_data     pdata;
        struct spi_device               *device;
        dev_t                           dev_num;
        char                            *buffer;
};

enum spi_device_names {
        test_spi_driver_first,
};

struct of_device_id test_spi_of_match[] = {
        {
                .compatible = "test_spi_driver",
                .data = (void*)test_spi_driver_first
        },
        { }
};

static const struct spi_device_id test_spi_id[] = {
        { "test_spi_driver", test_spi_driver_first },
        { }
};
MODULE_DEVICE_TABLE(spi, test_spi_id);

static struct spi_driver test_spi_driver = {
        .driver = {
                .name               = "test_spi_driver",
                .owner              = THIS_MODULE,
                .of_match_table     = of_match_ptr(test_spi_of_match),
        },

        .probe          = test_spi_probe,
        .remove         = test_spi_remove,
        .id_table       = test_spi_id,
};

/* Check device tree and get data*/
struct platform_device_data * get_platform_data_dt(struct spi_device *device)
{
        struct device *dev = &device->dev;
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

        if(of_property_read_string(dev_node, "test,name", &pdata->name)){
                dev_info(dev, "Missing serial number property \n");
                return ERR_PTR(-EINVAL);
        }

        if(of_property_read_u32(dev_node, "test,size", &pdata->size)){
                dev_info(dev, "Missing size property \n");
                return ERR_PTR(-EINVAL);
        }
        
        return pdata;
}

static int test_spi_probe(struct spi_device *device)
{
        struct device_private_data *dev_data;
        struct device *dev = &device->dev;
        struct platform_device_data *pdata;
        int err;

        device->bits_per_word = 8;
        device->mode = SPI_MODE_0;
        device->max_speed_hz = 500000;
        err = spi_setup(device);
        if(err < 0){
                dev_info(dev, "Failed to set SPI specifications");
                return err;
        }
        
        dev_info(dev, "SPI device detected\n");

        /* Check device tree and get data*/
        pdata = get_platform_data_dt(device);
        if(IS_ERR(pdata))
                return PTR_ERR(pdata);

        /* Allocate memory for device private data */
        dev_data = devm_kzalloc(dev, sizeof(*dev_data), GFP_KERNEL);
        if(!dev_data){
                dev_info(dev, "Cannot allocate memory for device private data\n");
                err = -ENOMEM;
        }

        /* Save spi device */
        dev_data->device = device;

        /* Save the device private data pointer in platform device structure */
        spi_set_drvdata(device, dev_data);

        /* Save data from DT */
        dev_data->pdata.size = pdata->size;
        dev_data->pdata.name= pdata->name;
        dev_info(dev, "Device size = %d\n", dev_data->pdata.size);
        dev_info(dev, "Device name = %s\n", dev_data->pdata.name);

        /*unsigned char buf[5] = "k0v4";
        test_spi_write(buf, 5, device);
        */

        dev_info(dev, "Probe function was successful\n");

        return 0;
}

static int test_spi_remove(struct spi_device *device)
{
        struct device *dev = &device->dev;

        dev_info(dev, "Remove Function is invoked...\n"); 

        return 0;
}


/* File operation functions */ 

static int test_spi_write(unsigned char *buf, unsigned int len, struct spi_device *device)
{
    int ret;
    /*
    int ret = spi_master_send(device, buf, len);
    */

    return ret;
}

static int test_spi_read(unsigned char *buf, unsigned int len, struct spi_device *device)
{
    int ret;
    /*
    int ret = spi_master_recv(device, buf, len);
    */

    return ret;
}


static int __init test_spi_init(void)
{
        int ret;
        
        ret = spi_register_driver(&test_spi_driver);
        if(ret != 0){
                pr_err("%s:driver registration failed spi-slave, error=%d\n", __func__, ret);
                spi_unregister_driver(&test_spi_driver);
                return ret;
        }

        pr_info("Platform SPI driver loaded\n");

        return 0;
}

static void __exit test_spi_cleanup(void)
{
        spi_unregister_driver(&test_spi_driver);
        pr_info("Module unloaded\n");
}

module_init(test_spi_init);
module_exit(test_spi_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim");
