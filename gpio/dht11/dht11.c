#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>

static int dht11_probe(struct platform_device *);
static int dht11_remove(struct platform_device *);

/* Device private data structure */
static struct device_private_data
{
        char label[20];
};

/* Driver private data structure */
static struct driver_private_data
{
        struct class *class_dht11;
        struct device *dev;
};

static struct driver_private_data dht11_driver_private_data;

/* Create device attributes */
static ssize_t temperature_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct device_private_data *device_data = dev_get_drvdata(dev);
        return sprintf(buf, "%s\n", device_data->label);
}

static ssize_t humidity_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct device_private_data *device_data = dev_get_drvdata(dev);
        return sprintf(buf, "%s\n", device_data->label);
}

static ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct device_private_data *device_data = dev_get_drvdata(dev);
        return sprintf(buf, "%s\n", device_data->label);
}

static DEVICE_ATTR_RO(temperature);
static DEVICE_ATTR_RO(humidity);
static DEVICE_ATTR_RO(label);

static struct attribute *gpio_attrs[] = 
{
        &dev_attr_temperature.attr,
        &dev_attr_humidity.attr,
        &dev_attr_label.attr,
        NULL
};

static struct attribute_group gpio_attr_group =
{
        .attrs = gpio_attrs
};

static const struct attribute_group *gpio_attr_groups[] = 
{
        &gpio_attr_group,
        NULL
};

static int platform_driver_sysfs_create_files(struct device *dev)
{
        int ret;

        ret = sysfs_create_group(&dev->kobj, &gpio_attr_group);
        return ret;
}


static struct of_device_id dht11_device_match[] = 
{
        {.compatible = "org,dht11"},
        {}
};

static struct platform_driver dht11_platform_driver = 
{
        .probe = dht11_probe,
        .remove = dht11_remove,
        .driver = {
            .name = "dht11",
            .of_match_table = of_match_ptr(dht11_device_match)
        }
};

static int dht11_probe(struct platform_device *pdev)
{
        const char *name;
        int ret;
        struct device *dev = &pdev->dev;
        struct device_node *dev_node = dev->of_node;
        struct device_private_data *device_data;

        if(of_property_read_string(dev_node, "label", device_data->label)){
                dev_warn(dev, "Missing lable information \n");
                return ERR_PTR(-EINVAL);
        }

        return 0;
}

static int dht11_remove(struct platform_device *pdev)
{
        int i;
        
        dev_info(&pdev->dev, "Remove called\n");

        for(i = 0; i < dht11_driver_private_data.count_of_devices; i++){
            device_unregister(dht11_driver_private_data.dev[i]);
        }

        return 0;
}

static int __init dht11_init(void)
{
        int ret;

        dht11_driver_private_data.class_dht11 = class_create(THIS_MODULE, "dht11");
        if(IS_ERR(dht11_driver_private_data.class_dht11)){
            pr_err("Class creation failed\n");
            ret = PTR_ERR(dht11_driver_private_data.class_dht11);
            return ret; 
        }

        platform_driver_register(&dht11_platform_driver);
        pr_info("Platform dht11 driver loaded\n");

        return 0;
}

static void __exit dht11_exit(void)
{
        platform_driver_unregister(&dht11_platform_driver);
        class_destroy(dht11_driver_private_data.class_dht11);

        pr_info("Module unloaded\n");
}

module_init(dht11_init);
module_exit(dht11_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("Control one led");
