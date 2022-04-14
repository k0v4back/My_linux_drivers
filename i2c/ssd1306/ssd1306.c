#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/sysfs.h>

static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int ssd1306_remove(struct i2c_client *client);
static int i2c_write(unsigned char *buf, unsigned int len);
static int i2c_read(unsigned char *buf, unsigned int len);
static ssize_t show_name(struct device *dev, struct device_attribute *attr, char *buf);
static int platform_driver_sysfs_create_files(struct device *pcd_dev);
static int SSD1306_DisplayInit(void);
static void SSD1306_Write(bool is_cmd, unsigned char data);
static void SSD1306_SetCursor( uint8_t lineNo, uint8_t cursorPos);
static void SSD1306_String(unsigned char *str);
static void SSD1306_PrintChar(unsigned char c);
static void SSD1306_Fill(unsigned char data);
static void SSD1306_GoToNextLine(void);

#define SSD1306_SLAVE_ADDR      (       0x3C )              // SSD1306 OLED Slave Address
#define SSD1306_MAX_SEG         (        128 )              // Maximum segment
#define SSD1306_MAX_LINE        (          7 )              // Maximum line
#define SSD1306_DEF_FONT_SIZE   (          5 )              // Default font size

static uint8_t SSD1306_LineNum   = 0;
static uint8_t SSD1306_CursorPos = 0;
static uint8_t SSD1306_FontSize  = 5;

static struct i2c_client *client_global = NULL;  // I2C Cient Structure (In our case it is OLED)

static const unsigned char ssd1306_font[][5]= 
{
        {0x00, 0x00, 0x00, 0x00, 0x00},   // space
        {0x00, 0x00, 0x2f, 0x00, 0x00},   // !
        {0x00, 0x07, 0x00, 0x07, 0x00},   // "
        {0x14, 0x7f, 0x14, 0x7f, 0x14},   // #
        {0x24, 0x2a, 0x7f, 0x2a, 0x12},   // $
        {0x23, 0x13, 0x08, 0x64, 0x62},   // %
        {0x36, 0x49, 0x55, 0x22, 0x50},   // &
        {0x00, 0x05, 0x03, 0x00, 0x00},   // '
        {0x00, 0x1c, 0x22, 0x41, 0x00},   // (
        {0x00, 0x41, 0x22, 0x1c, 0x00},   // )
        {0x14, 0x08, 0x3E, 0x08, 0x14},   // *
        {0x08, 0x08, 0x3E, 0x08, 0x08},   // +
        {0x00, 0x00, 0xA0, 0x60, 0x00},   // ,
        {0x08, 0x08, 0x08, 0x08, 0x08},   // -
        {0x00, 0x60, 0x60, 0x00, 0x00},   // .
        {0x20, 0x10, 0x08, 0x04, 0x02},   // /

        {0x3E, 0x51, 0x49, 0x45, 0x3E},   // 0
        {0x00, 0x42, 0x7F, 0x40, 0x00},   // 1
        {0x42, 0x61, 0x51, 0x49, 0x46},   // 2
        {0x21, 0x41, 0x45, 0x4B, 0x31},   // 3
        {0x18, 0x14, 0x12, 0x7F, 0x10},   // 4
        {0x27, 0x45, 0x45, 0x45, 0x39},   // 5
        {0x3C, 0x4A, 0x49, 0x49, 0x30},   // 6
        {0x01, 0x71, 0x09, 0x05, 0x03},   // 7
        {0x36, 0x49, 0x49, 0x49, 0x36},   // 8
        {0x06, 0x49, 0x49, 0x29, 0x1E},   // 9

        {0x00, 0x36, 0x36, 0x00, 0x00},   // :
        {0x00, 0x56, 0x36, 0x00, 0x00},   // ;
        {0x08, 0x14, 0x22, 0x41, 0x00},   // <
        {0x14, 0x14, 0x14, 0x14, 0x14},   // =
        {0x00, 0x41, 0x22, 0x14, 0x08},   // >
        {0x02, 0x01, 0x51, 0x09, 0x06},   // ?
        {0x32, 0x49, 0x59, 0x51, 0x3E},   // @

        {0x7C, 0x12, 0x11, 0x12, 0x7C},   // A
        {0x7F, 0x49, 0x49, 0x49, 0x36},   // B
        {0x3E, 0x41, 0x41, 0x41, 0x22},   // C
        {0x7F, 0x41, 0x41, 0x22, 0x1C},   // D
        {0x7F, 0x49, 0x49, 0x49, 0x41},   // E
        {0x7F, 0x09, 0x09, 0x09, 0x01},   // F
        {0x3E, 0x41, 0x49, 0x49, 0x7A},   // G
        {0x7F, 0x08, 0x08, 0x08, 0x7F},   // H
        {0x00, 0x41, 0x7F, 0x41, 0x00},   // I
        {0x20, 0x40, 0x41, 0x3F, 0x01},   // J
        {0x7F, 0x08, 0x14, 0x22, 0x41},   // K
        {0x7F, 0x40, 0x40, 0x40, 0x40},   // L
        {0x7F, 0x02, 0x0C, 0x02, 0x7F},   // M
        {0x7F, 0x04, 0x08, 0x10, 0x7F},   // N
        {0x3E, 0x41, 0x41, 0x41, 0x3E},   // O
        {0x7F, 0x09, 0x09, 0x09, 0x06},   // P
        {0x3E, 0x41, 0x51, 0x21, 0x5E},   // Q
        {0x7F, 0x09, 0x19, 0x29, 0x46},   // R
        {0x46, 0x49, 0x49, 0x49, 0x31},   // S
        {0x01, 0x01, 0x7F, 0x01, 0x01},   // T
        {0x3F, 0x40, 0x40, 0x40, 0x3F},   // U
        {0x1F, 0x20, 0x40, 0x20, 0x1F},   // V
        {0x3F, 0x40, 0x38, 0x40, 0x3F},   // W
        {0x63, 0x14, 0x08, 0x14, 0x63},   // X
        {0x07, 0x08, 0x70, 0x08, 0x07},   // Y
        {0x61, 0x51, 0x49, 0x45, 0x43},   // Z

        {0x00, 0x7F, 0x41, 0x41, 0x00},   // [
        {0x55, 0xAA, 0x55, 0xAA, 0x55},   // Backslash (Checker pattern)
        {0x00, 0x41, 0x41, 0x7F, 0x00},   // ]
        {0x04, 0x02, 0x01, 0x02, 0x04},   // ^
        {0x40, 0x40, 0x40, 0x40, 0x40},   // _
        {0x00, 0x03, 0x05, 0x00, 0x00},   // `

        {0x20, 0x54, 0x54, 0x54, 0x78},   // a
        {0x7F, 0x48, 0x44, 0x44, 0x38},   // b
        {0x38, 0x44, 0x44, 0x44, 0x20},   // c
        {0x38, 0x44, 0x44, 0x48, 0x7F},   // d
        {0x38, 0x54, 0x54, 0x54, 0x18},   // e
        {0x08, 0x7E, 0x09, 0x01, 0x02},   // f
        {0x18, 0xA4, 0xA4, 0xA4, 0x7C},   // g
        {0x7F, 0x08, 0x04, 0x04, 0x78},   // h
        {0x00, 0x44, 0x7D, 0x40, 0x00},   // i
        {0x40, 0x80, 0x84, 0x7D, 0x00},   // j
        {0x7F, 0x10, 0x28, 0x44, 0x00},   // k
        {0x00, 0x41, 0x7F, 0x40, 0x00},   // l
        {0x7C, 0x04, 0x18, 0x04, 0x78},   // m
        {0x7C, 0x08, 0x04, 0x04, 0x78},   // n
        {0x38, 0x44, 0x44, 0x44, 0x38},   // o
        {0xFC, 0x24, 0x24, 0x24, 0x18},   // p
        {0x18, 0x24, 0x24, 0x18, 0xFC},   // q
        {0x7C, 0x08, 0x04, 0x04, 0x08},   // r
        {0x48, 0x54, 0x54, 0x54, 0x20},   // s
        {0x04, 0x3F, 0x44, 0x40, 0x20},   // t
        {0x3C, 0x40, 0x40, 0x20, 0x7C},   // u
        {0x1C, 0x20, 0x40, 0x20, 0x1C},   // v
        {0x3C, 0x40, 0x30, 0x40, 0x3C},   // w
        {0x44, 0x28, 0x10, 0x28, 0x44},   // x
        {0x1C, 0xA0, 0xA0, 0xA0, 0x7C},   // y
        {0x44, 0x64, 0x54, 0x4C, 0x44},   // z

        {0x00, 0x10, 0x7C, 0x82, 0x00},   // {
        {0x00, 0x00, 0xFF, 0x00, 0x00},   // |
        {0x00, 0x82, 0x7C, 0x10, 0x00},   // }
        {0x00, 0x06, 0x09, 0x09, 0x06}    // ~ (Degrees)
};

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

/*
 * ssd1306 oled display functions
 */
static int SSD1306_DisplayInit(void)
{
        msleep(100);

        /*
         * Commands to initialize the SSD_1306 OLED Display
         */
        SSD1306_Write(true, 0xAE); // Entire Display OFF
        SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
        SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
        SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
        SSD1306_Write(true, 0x3F); // 64 COM lines
        SSD1306_Write(true, 0xD3); // Set display offset
        SSD1306_Write(true, 0x00); // 0 offset
        SSD1306_Write(true, 0x40); // Set first line as the start line of the display
        SSD1306_Write(true, 0x8D); // Charge pump
        SSD1306_Write(true, 0x14); // Enable charge dump during display on
        SSD1306_Write(true, 0x20); // Set memory addressing mode
        SSD1306_Write(true, 0x00); // Horizontal addressing mode
        SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
        SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
        SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
        SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
        SSD1306_Write(true, 0x81); // Set contrast control
        SSD1306_Write(true, 0x80); // Set Contrast to 128
        SSD1306_Write(true, 0xD9); // Set pre-charge period
        SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
        SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
        SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
        SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
        SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
        SSD1306_Write(true, 0x2E); // Deactivate scroll
        SSD1306_Write(true, 0xAF); // Display ON in normal mode

        //Clear the display
        SSD1306_Fill(0x00);

        return 0;
}

static void SSD1306_Write(bool is_cmd, unsigned char data)
{
        unsigned char buf[2] = {0};
        int ret;

        if( is_cmd == true )
        {
                buf[0] = 0x00;
        }else{
                buf[0] = 0x40;
        }

        buf[1] = data;

        ret = i2c_write(buf, 2);
}

static void SSD1306_SetCursor( uint8_t lineNo, uint8_t cursorPos)
{
        /* Move the Cursor to specified position only if it is in range */
        if((lineNo <= SSD1306_MAX_LINE) && (cursorPos < SSD1306_MAX_SEG)){
                SSD1306_LineNum   = lineNo;             // Save the specified line number
                SSD1306_CursorPos = cursorPos;          // Save the specified cursor position

                SSD1306_Write(true, 0x21);              // cmd for the column start and end address
                SSD1306_Write(true, cursorPos);         // column start addr
                SSD1306_Write(true, SSD1306_MAX_SEG-1); // column end addr

                SSD1306_Write(true, 0x22);              // cmd for the page start and end address
                SSD1306_Write(true, lineNo);            // page start addr
                SSD1306_Write(true, SSD1306_MAX_LINE);  // page end addr
        }
}

static void SSD1306_String(unsigned char *str)
{
        while(*str){
                SSD1306_PrintChar(*str++);
        }
}

static void SSD1306_PrintChar(unsigned char c)
{
        uint8_t data_byte;
        uint8_t temp = 0;

        /*
        * If we character is greater than segment len or we got new line charcter
        * then move the cursor to the new line
        */ 
        if(((SSD1306_CursorPos + SSD1306_FontSize) >= SSD1306_MAX_SEG) || (c == '\n')){
                SSD1306_GoToNextLine();
        }
        
        // print charcters other than new line
        if( c != '\n' ){
                c -= 0x20;  //or c -= ' ';
                do{
                        data_byte= ssd1306_font[c][temp]; // Get the data to be displayed from LookUptable

                        SSD1306_Write(false, data_byte);  // write data to the OLED
                        SSD1306_CursorPos++;

                        temp++;

                }while(temp < SSD1306_FontSize);
                
                SSD1306_Write(false, 0x00);         //Display the data
                SSD1306_CursorPos++;
        }
}

static void SSD1306_Fill(unsigned char data)
{
        unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
        unsigned int i      = 0;

        //Fill the Display
        for(i = 0; i < total; i++){
                SSD1306_Write(false, data);
        }
}

static void  SSD1306_GoToNextLine( void )
{
        /*
         * Increment the current line number.
         * roll it back to first line, if it exceeds the limit. 
         */
        SSD1306_LineNum++;
        SSD1306_LineNum = (SSD1306_LineNum & SSD1306_MAX_LINE);

        SSD1306_SetCursor(SSD1306_LineNum,0); /* Finally move it to next line */
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
        client_global = client;

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

        SSD1306_DisplayInit();
        SSD1306_SetCursor(0,0);  
        SSD1306_String("Welcome\nTo\nEmbeTronicX\n\n");

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

static int i2c_write(unsigned char *buf, unsigned int len)
{
    int ret = i2c_master_send(client_global, buf, len);

    return ret;
}

static int i2c_read(unsigned char *buf, unsigned int len)
{
    int ret = i2c_master_recv(client_global, buf, len);

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
