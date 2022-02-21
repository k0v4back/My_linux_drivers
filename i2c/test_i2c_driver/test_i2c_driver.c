#include <linux/module.h>
#include <linux/init.h> 
#include <linux/i2c.h> 
#include <linux/fs.h>
#include <linux/err.h> 
#include <linux/of.h>
#include <linux/of_device.h>

enum i2c_device_names {
        test_i2c_driver,
};

static const struct i2c_device_id test_i2c_id[] = {
        { "test_i2c_driver", test_i2c_driver },
        { },
};
MODULE_DEVICE_TABLE(i2c, test_i2c_id);

static struct i2c_driver test_i2c_driver = {
        .driver = {
            .name   = "test_i2c_driver",
        },

        .probe          = test_i2c_probe,
        .remove         = test_i2c_remove,
        .id_table       = test_i2c_id,
};

static const struct of_device_id test_i2c_of_match[] = { 
        {
            .compatible = "org,test_i2c_driver",
            .data = (void *)test_i2c_driver
        },
        { },
};

static int test_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        return 0;
}

static int test_i2c_remove(struct i2c_client *client)
{
        return 0;
}

static int __init test_i2c_init(void)
{
        return i2c_add_driver(&test_i2c_driver);
}

static void __exit test_i2c_cleanup(void)
{
        i2c_del_driver(&test_i2c_driver);
}

module_init(test_i2c_init);
module_exit(test_i2c_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim");
