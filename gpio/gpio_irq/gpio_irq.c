/*
 * gpio_irq.c - Interruption when the button is pressed.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/sched/signal.h>

#define SIG_PUSH_BUTTON 44

/* Store information about user-space application */
#define REGISTER_UAPP _IO('R', 'g')
static struct task_struct *task = NULL;

unsigned int irq_number;
unsigned int gpio_number = 65; /* GPIO2_1 = 32*2+1= 65 */

dev_t dev_num = 0;
struct cdev gpio_irq_signal_cdev;
struct class *class_gpio_irq_signal;
struct device *device_gpio_irq_signal;

static irq_handler_t gpio_irq_handler(
        unsigned int irq, void *dev_id, struct pt_regs *regs)
{
        struct siginfo info;

        pr_info("ISR was called!\n");

        if(task != NULL) {
                memset(&info, 0, sizeof(info));
                info.si_signo = SIG_PUSH_BUTTON;
                info.si_code = SI_QUEUE;

                // Send the signal
                if(send_sig_info(SIG_PUSH_BUTTON, 
                            (struct kernel_siginfo *) &info, task) < 0) 
                        printk("gpio_irq_signal: Error sending signal\n");
        }

        return (irq_handler_t)IRQ_HANDLED; 
}

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg) {
        if(cmd == REGISTER_UAPP) {
                task = get_current();
                printk("gpio_irq_signal: Userspace app with PID %d is registered\n", task->pid);
        }

        return 0;
}

static int my_close(struct inode *device_file, struct file *instance) {
        if(task != NULL)
                task = NULL;

        return 0;
}

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .release = my_close,
        .unlocked_ioctl = my_ioctl,
};

static int __init gpio_irq_init(void)
{
        int ret;

        /* Dynamically allocating Major number */
        ret = alloc_chrdev_region(&dev_num, 0, 1, "gpio_irq_signal"); 
        if(ret < 0) goto out;

        printk(KERN_INFO "Device number <major>:<minor> = %d:%d \n",
                MAJOR(dev_num), MINOR(dev_num));

         /* Register a device cdev struct with VFS */
        cdev_init(&gpio_irq_signal_cdev, &fops);
        gpio_irq_signal_cdev.owner = THIS_MODULE;
        ret = cdev_add(&gpio_irq_signal_cdev, dev_num, 1);
        if(ret < 0){
                printk(KERN_ERR "cdev_add() faild!");
                goto unreg_chrdev;
        }

        /* Create device class under /sys/class */
        class_gpio_irq_signal = class_create(THIS_MODULE, "gpio_irq_signal");
        if(IS_ERR(class_gpio_irq_signal)){
                printk(KERN_ERR "class_create() faild!");
                goto cdev_del;
        }

        /* Populate the sysfs with device information. udevd will create /dev/gpio_irq_signal */
        device_gpio_irq_signal = device_create(class_gpio_irq_signal,
                NULL, dev_num, NULL, "gpio_irq_signal");
        if(IS_ERR(device_gpio_irq_signal)){
                printk(KERN_ERR "Device create faild \n");
                goto class_del;
        }

        /* Setup the gpio */
        if(gpio_request(gpio_number, "rpi-gpio-65")) {
                pr_info("Error!\nCan not allocate GPIO 65\n");
                return -EINVAL;
        }

        /* Set GPIO 65 direction */
        if(gpio_direction_input(gpio_number)) {
                pr_info("Error!\nCan not set GPIO 65 to input!\n");
                gpio_free(gpio_number);
                return -EINVAL;
        }

        /* Setup the interrupt */
        irq_number = gpio_to_irq(gpio_number);

        if(request_irq(irq_number, (irq_handler_t)gpio_irq_handler, 
                    IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0)
        {
                printk("Error!\nCan not request interrupt: %d\n", irq_number);
                gpio_free(gpio_number);
                return -EINVAL;
        }

        printk("GPIO 65 is mapped to IRQ Nr.: %d\n", irq_number);

        return 0;

class_del:
        class_destroy(class_gpio_irq_signal);
cdev_del:
        cdev_del(&gpio_irq_signal_cdev);
unreg_chrdev:
        unregister_chrdev_region(dev_num, 1);
out:
        return ret;
}

static void __exit gpio_irq_exit(void)
{
        device_destroy(class_gpio_irq_signal, dev_num);
        class_destroy(class_gpio_irq_signal);
        cdev_del(&gpio_irq_signal_cdev);
        unregister_chrdev_region(dev_num, 1);

        free_irq(irq_number, NULL);
        gpio_free(gpio_number);

        pr_info("gpio_irq module unloaded\n");
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("Push button, get irq");
