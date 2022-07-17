#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>

#define CHANNEL_PWM_CONTROLLER 1

static int led_pwm_probe(struct platform_device *pdev);
static int led_pwm_remove(struct platform_device *pdev);

struct pwm_device *pwm0 = NULL;
u32 pwm_on_time = 500000000;

struct led_pwm_device {
        struct pwm_chip chip;
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
        },
        .probe  = led_pwm_probe,
        .remove = led_pwm_remove,
};

static int led_pwm_config(struct pwm_chip *chip,
            struct pwm_device *pwm_dev,
            int duty_ns, int period_ns)
{
        int ret = 0;
        struct device *dev = chip->dev; 

        duty_ns = 1000000000;
        period_ns = 500000000;

        ret = pwm_config(pwm_dev, duty_ns, period_ns);
        if (ret != 0)
                dev_info(dev, "Cannot config PWM chip\n");

        return ret;
}

static int led_pwm_enable(struct pwm_chip *chip,
             struct pwm_device *pwm_dev)
{
        pr_info("Enable PWM device number %d", pwm_dev->hwpwm);

        return 0;
}

static void led_pwm_disable(struct pwm_chip *chip,
            struct pwm_device *pwm_dev)
{
        pr_info("Disabled PWM device number %d", pwm_dev->hwpwm);
}

static const struct pwm_ops led_pwm_ops = {
        .config     = led_pwm_config,
        .enable     = led_pwm_enable,
        .disable    = led_pwm_disable,
        .owner      = THIS_MODULE,
};

static int led_pwm_probe(struct platform_device *pdev)
{
        struct led_pwm_device *led_pwm_priv;
        struct device *dev = &pdev->dev;
        int ret = 0;

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
