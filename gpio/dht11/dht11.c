#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/gpio/driver.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#define GPIO_HIGH   1
#define GPIO_LOW    0

enum DHT11_RESPONSE {
        ERROR_DHT11_FIRST_INIT,
        ERROR_DHT11_SECOND_INIT,
        SUCCESS_DHT11_INIT
};

/* Device private data structure */
static struct device_private_data
{
        const char *name;
        int temperature;
        int humidity;
        struct gpio_desc *desc;
};

/* Driver private data structure */
static struct driver_private_data
{
        struct class *class_dht11;
        struct device *dev;
};

static int dht11_probe(struct platform_device *);
static int dht11_remove(struct platform_device *);
static int dht11_get_data(struct device_private_data *);

struct driver_private_data dht11_driver_private_data;

/* Create device attributes */
static ssize_t temperature_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        int ret = 0;
        struct device_private_data *dev_data = dev_get_drvdata(dev);

        ret = sprintf(buf, "%d\n", dev_data->temperature);

        return ret;
}

static ssize_t humidity_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        int ret = 0;
        struct device_private_data *dev_data = dev_get_drvdata(dev);

        ret = sprintf(buf, "%d\n", dev_data->humidity);

        return ret;
}

static ssize_t show_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        int ret = 0;
        struct device_private_data *dev_data = dev_get_drvdata(dev);

        ret = sprintf(buf, "%s\n", dev_data->name);

        return ret;
}

static DEVICE_ATTR_RO(temperature);
static DEVICE_ATTR_RO(humidity);
static DEVICE_ATTR_RO(show);

static struct attribute *gpio_attrs[] = 
{
        &dev_attr_temperature.attr,
        &dev_attr_humidity.attr,
        &dev_attr_show.attr,
        NULL
};

static struct attribute_group gpio_dht11_attr_group =
{
        .attrs = gpio_attrs
};

static const struct attribute_group *gpio_dht11_attr_groups[] = 
{
        &gpio_dht11_attr_group,
        NULL
};

static int platform_driver_sysfs_create_files(struct device *dev)
{
        int ret;

        ret = sysfs_create_group(&dev->kobj, &gpio_dht11_attr_group);
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

/*
 * Check device tree and get data
 */
static struct device_private_data * get_platform_data_from_dt(struct device *dev)
{
        struct device_node *dev_node = dev->of_node;
        struct device_private_data *device_data;

        if(!dev_node)
                return NULL;

        /* Allocate memory for device_data */
        device_data = devm_kzalloc(dev, sizeof(struct device_private_data), GFP_KERNEL);
        if(!device_data){
                dev_info(dev, "Cannot allocate memory for platform_device_data\n");
                return ERR_PTR(-ENOMEM);
        }

        if(of_property_read_string(dev_node, "dht11,name", &device_data->name)){
                dev_info(dev, "Missing name property \n");
                return ERR_PTR(-EINVAL);
        }

        return device_data;
}


static int dht11_probe(struct platform_device *pdev)
{
        const char *name;
        int ret;
        struct device *dev = &pdev->dev;
        struct device_private_data *device_data;

        device_data = get_platform_data_from_dt(dev);
        if(IS_ERR(device_data))
                return PTR_ERR(device_data);
        
        /* Checking for the existence of a configuration for the pin */
        device_data->desc = gpiod_get(dev, "data_pin", GPIOD_OUT_LOW);
        if(IS_ERR(device_data->desc)){
                dev_err(dev, "GPIO error\n");
                return -EINVAL;
        }
        
        dht11_get_data(device_data);

        return 0;
}

static int dht11_remove(struct platform_device *pdev)
{
        struct device *dev = &pdev->dev;
        
        /* Delete existing sysfs group */
        sysfs_remove_group(&dev->kobj, &gpio_dht11_attr_group);
        dev_info(dev, "Remove called\n");

        return 0;
}

/* Interaction with the sensor */

static int dht11_init(struct device_private_data *device_data)
{
        /* set output mode */
        gpiod_direction_output(device_data->desc, 0);

        /* Sending Ready command to start working with sensor */
        gpiod_set_value(device_data->desc, GPIO_LOW);
        mdelay(20);
        gpiod_set_value(device_data->desc, GPIO_HIGH);

        /* set input mode */
        gpiod_direction_input(device_data->desc);

        /* Checking if the sensor responde */
        udelay(40);
        if(gpio_get_value(device_data->desc) == GPIO_HIGH)
                return ERROR_DHT11_FIRST_INIT;

        udelay(80);
        if(gpio_get_value(device_data->desc) == GPIO_LOW)
                return ERROR_DHT11_SECOND_INIT;

        /* Wait until the sensor starts transmitting data (pull the line to the ground) */
        while(gpio_get_value(device_data->desc) == GPIO_HIGH){};

        return SUCCESS_DHT11_INIT;
}

static int dht11_read_byte(struct device_private_data *device_data)
{
        u8 i = 8, byte = 0;

        /* set input mode */
        gpiod_direction_input(device_data->desc);

        while(i--){
                /* Wait until pull the line to up */
                while(gpio_get_value(device_data->desc) == GPIO_LOW){};

                udelay(40);

                if(gpio_get_value(device_data->desc) == GPIO_HIGH){
                        byte |= (1 << i);
                        /* Wait until pull the line to ground */
                        while(gpio_get_value(device_data->desc) == GPIO_HIGH){};
                }
        }

        return byte;
}

static int dht11_get_data(struct device_private_data *device_data)
{
        int ret = 0;
        int i = 0;
        u8 data[5] = {0};

        ret = dht11_init(device_data);
        if(ret != SUCCESS_DHT11_INIT){
                pr_err("Error with dht11 init\n");
                return ret;
        }

        /* Read bytes */
        for(i = 0; i < 5; i++){
                data[i] = dht11_read_byte(device_data);
        }

        /* Check control summ */
        if((data[0] + data[1] + data[2] + data[3]) == data[4]){
                device_data->humidity = data[0];
                device_data->temperature = data[2];
        }

        return ret;
}


static int __init dht11_sysfs_init(void)
{
        int ret;

        dht11_driver_private_data.class_dht11 = class_create(THIS_MODULE, "dht11");
        if(IS_ERR(dht11_driver_private_data.class_dht11)){
            pr_err("Class creation failed\n");
            return ret; 
        }

        platform_driver_register(&dht11_platform_driver);
        pr_info("Platform dht11 driver loaded success\n");

        return 0;
}

static void __exit dht11_exit(void)
{
        platform_driver_unregister(&dht11_platform_driver);
        class_destroy(dht11_driver_private_data.class_dht11);

        pr_info("Module unloaded\n");
}

module_init(dht11_sysfs_init);
module_exit(dht11_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("DHT11 get temperature and humidity");
