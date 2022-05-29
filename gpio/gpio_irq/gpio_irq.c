/*
 * gpio_irq.c - Interruption when the button is pressed.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/sched/signal.h>

#define SIG_PUSH_BUTTON 44

/* Store information about user-space application */
static struct task_struct *task = NULL;

unsigned int irq_number;
unsigned int gpio_number = 65; /* GPIO2_1 = 32*2+1= 65 */

static irq_handler_t gpio_irq_handler(
        unsigned int irq, void *dev_id, struct pt_regs *regs)
{
        struct siginfo info;

        pr_info("ISR was called!\n");

        if(task != NULL) {
                memset(&info, 0, sizeof(info));
                info.si_signo = SIG_PUSH_BUTTON;
                info.si_code = SI_QUEUE;

                /* Send the signal */
                if(send_sig_info(SIG_PUSH_BUTTON, 
                            (struct kernel_siginfo *) &info, task) < 0) 
                        printk("gpio_irq_signal: Error sending signal\n");
        }

        return (irq_handler_t)IRQ_HANDLED;
}

static int __init gpio_irq_init(void)
{
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
}

static void __exit gpio_irq_exit(void)
{
        free_irq(irq_number, NULL);
        gpio_free(gpio_number);

        pr_info("gpio_irq module unloaded\n");
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kosolapov Vadim (https://github.com/k0v4back)");
MODULE_DESCRIPTION("Push button, get irq");
