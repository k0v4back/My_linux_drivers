#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>

#define CHANNEL_PWM_CONTROLLER 1

static int led_pwm_probe(struct platform_device *pdev);
static int led_pwm_remove(struct platform_device *pdev);
struct platform_device_data * get_platform_data_dt(
        struct device *dev);

struct led_pwm_device {
        struct pwm_chip chip;
};

struct led_pwm {
       const char *name; 
};

static struct of_device_id led_pwm_dt_ids[] = {
        {
                .compatible = "led_pwm",
                .data = 0
        },
        { }
};
MODULE_DEVICE_TABLE(of, led_pwm_dt_ids);

static struct platform_driver led_pwm_driver = {
        .driver = {
                .name               = "led_pwm",
                .owner              = THIS_MODULE,
                .of_match_table     = of_match_ptr(led_pwm_dt_ids),
        },
        .probe  = led_pwm_probe,
        .remove = led_pwm_remove,
};

struct platform_device_data * get_platform_data_dt(
        struct device *dev)
{
        struct device_node *dev_node = dev->of_node;
        struct led_pwm *data_led_pwm;

        if(!dev_node)
                return NULL;

        data_led_pwm = devm_kzalloc(dev, sizeof(*data_led_pwm),
                GFP_KERNEL);
        if (!data_led_pwm) {
                dev_info(dev, "Cannot allocate memory for led_pwm\n");
                return ERR_PTR(-ENOMEM);
        }

        if (of_property_read_string(dev_node,
                    "led_pwm,name", &data_led_pwm->name)) {
                dev_info(dev, "Missing name property \n");
                return ERR_PTR(-EINVAL);
        }

        return data_led_pwm;
}

static int led_pwm_probe(struct platform_device *pdev)
{
        struct led_pwm_device *led_pwm_priv;
        struct led_pwm *data_led_pwm;
        struct device *dev = &pdev->dev;
        int ret = 0;

        data_led_pwm = get_platform_data_dt(&dev);
        if (IS_ERR(data_led_pwm))
                return PTR_ERR(data_led_pwm);

        led_pwm_priv = devm_kzalloc(dev, sizeof(*led_pwm_priv),
                                GFP_KERNEL);
        if (!led_pwm_priv) {
                dev_info(dev, "Cannot allocate memory\n");
                return -ENOMEM;
        }

        led_pwm_priv->chip.ops = &led_pwm_ops;
        led_pwm_priv->chip.dev = dev;
        led_pwm_priv->chip.base = -1;
        led_pwm_priv->chip.npwm = CHANNEL_PWM_CONTROLLER;

        platform_set_drvdata(pdev, led_pwm_priv);

        /* Register PWM chip */
        ret = pwmchip_add(&led_pwm_priv->chip);
        if (ret != 0)
                dev_info(dev, "Cannot register PWM chip\n");

        pr_info("Probe function called\n");

        return 0;
}

static int led_pwm_remove(struct platform_device *pdev)
{
        struct led_pwm_device *led_pwm_priv;
        struct device *dev = &pdev->dev;
        int ret = 0;

        led_pwm_priv = platform_get_drvdata(pdev);

        /* Remove PWM chip */
        ret = pwmchip_remove(&led_pwm_priv->chip);
        if (ret != 0)
                dev_info(dev, "Cannot remove PWM chip\n");

        pr_info("Remove function called\n");

        return 0;
}

static int __init led_pwm_init(void)
{
        platform_driver_register(&led_pwm_driver);

        pr_info("Init function called\n");

        return 0;
}

static void __exit led_pwm_exit(void)
{
        platform_driver_unregister(&led_pwm_driver);

        pr_info("Exit function called\n");
}

module_init(led_pwm_init);
module_exit(led_pwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("PWM led driver");
