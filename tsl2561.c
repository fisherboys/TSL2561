/******************** (C) COPYRIGHT 2017 YU **************************
 * FILE NAME        ：TSL2561
 * DESCRIPTION      ：LINUX DRIVER FOR TSL2561(GY2561)
 * KERNEL           ：LINUX 3.0.35
 * SYSTEM           ：ANDROID 4.4.2
 * VERSION          ：1.0
 * RELEASE DATE     ：2017/09/20
 * PLATFORM         ：IMX6Q
 * AUTHOR           ：YU
*********************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#include "tsl2561.h"

#define DEBUG 1

#ifndef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) (void)(0)
#endif


#define TSL_MAGIC 'T'
//#define I2C_WRITE _IOW('T',0,int)
//#define I2C_READ _IOW('T',1,int)
#define GET_LUX _IOW('T', 0, int)

struct i2c_client *tsl2561_client; 

struct tsl2561_data
{
    unsigned char addr;
    unsigned char data;
};

/*
FUNCTION: START TSL2561
*/
void TSL2561_PowerOn(void)
{
    i2c_smbus_write_byte_data(tsl2561_client, TSL2561_CMD, TSL2561_POWER_ON);
}

/*
FUNCTION: POWER DOWN TSL2561
*/
void TSL2561_PowerDown(void)
{
    i2c_smbus_write_byte_data(tsl2561_client, TSL2561_CMD, TSL2561_POWER_DOWN);
}

/*
FUNCTION: SET INTEGRATION TIME
PARA:TIMING_13MS, TIMING_101MS, TIMING_402MS
*/
void TSL2561_TimingSet(unsigned char TIME)
{
    i2c_smbus_write_byte_data(tsl2561_client, TSL2561_TIMING, TIME);
}

/*
FUNCTION: INIT TSL2561
PARA:TIMING_13MS, TIMING_101MS, TIMING_402MS
*/
void TSL2561_Init(unsigned char TIME)
{
    TSL2561_PowerOn();
    
    TSL2561_TimingSet(TIME);
}

/*
FUNCTION: GET CHANEL_VAL
PARA:TSL2561_DATA0_LOW, TSL2561_DATA0_HIGH, TSL2561_DATA0_LOW, TSL2561_DATA0_HIGH
*/
unsigned int TSL2561_ChannelRead(unsigned char Low_Addr, unsigned char High_Addr)
{
    unsigned int Channel_Val;
    unsigned char Low_Val, High_Val;

    Low_Val = i2c_smbus_read_byte_data(tsl2561_client, Low_Addr);
    High_Val = i2c_smbus_read_byte_data(tsl2561_client, High_Addr);

    Channel_Val = High_Val * 256 + Low_Val;

    return Channel_Val;
}



// lux equation approximation without floating point calculations
//////////////////////////////////////////////////////////////////////////////
// Routine: unsigned int CalculateLux(unsigned int ch0, unsigned int ch0, int iType)
//
// Description: Calculate the approximate illuminance (lux) given the raw
// channel values of the TSL2560. The equation if implemented
// as a piece?wise linear approximation.
//
// Arguments: unsigned int iGain ? gain, where 0:1X, 1:16X
// unsigned int tInt - integration time, where 0:13.7mS, 1:100mS, 2:402mS,
// 3:Manual
// unsigned int ch0 - raw channel value from channel 0 of TSL2561
// unsigned int ch1 - raw channel value from channel 1 of TSL2561
// unsigned int iType - package type (T or CS)
//
// Return: unsigned int - the approximate illuminance (lux)
//
//////////////////////////////////////////////////////////////////////////////
unsigned int CalculateLux(unsigned int iGain, 
                          unsigned int tInt,
                          unsigned int ch0,
                          unsigned int ch1, 
                          int iType)
{
//----------------------------------------------------------------------------
// first, scale the channel values depending on the gain and integration time
// 16X, 402mS is nominal.
// scale if integration time is NOT 402 msec
    unsigned long chScale;
    unsigned long channel1;
    unsigned long channel0;
    unsigned long ratio1 = 0;// protect against divide by zero
    unsigned long ratio;
    unsigned int b, m;
    unsigned long temp;
    unsigned long lux;
    switch (tInt)
    {   
        case 0: // 13.7 msec
        chScale = CHSCALE_TINT0;
        break;
        case 1: // 101 msec
        chScale = CHSCALE_TINT1;
        break;
        default: // assume no scaling
        chScale = (1 << CH_SCALE);
        break;
    }
    // scale if gain is NOT 16X
    if (!iGain) chScale = chScale << 4; // scale 1X to 16X
    // scale the channel values
    channel0 = (ch0 * chScale) >> CH_SCALE;
    channel1 = (ch1 * chScale) >> CH_SCALE;
    //----------------------------------------------------------------------
    // find the ratio of the channel values (Channel1/Channel0)
    // protect against divide by zero
    //unsigned long ratio1 = 0;
    if (channel0 != 0) ratio1 = (channel1 << (RATIO_SCALE+1)) / channel0;
    // round the ratio value
     ratio = (ratio1 + 1) >> 1;
    // is ratio <= eachBreak ?
    //unsigned int b, m;
    switch (iType)
    {
        case 0: // T, FN and CL package
            if ((ratio >= 0) && (ratio <= K1T))
                {b=B1T; m=M1T;}
            else if (ratio <= K2T)
                {b=B2T; m=M2T;}
            else if (ratio <= K3T)
                {b=B3T; m=M3T;}
            else if (ratio <= K4T)
                {b=B4T; m=M4T;}
            else if (ratio <= K5T)
                {b=B5T; m=M5T;}
            else if (ratio <= K6T)
                {b=B6T; m=M6T;}
            else if (ratio <= K7T)
                {b=B7T; m=M7T;}
            else if (ratio > K8T)
                {b=B8T; m=M8T;}
            break;
        case 1:// CS package
            if ((ratio >= 0) && (ratio <= K1C))
                {b=B1C; m=M1C;}
            else if (ratio <= K2C)
                {b=B2C; m=M2C;}
            else if (ratio <= K3C)
                {b=B3C; m=M3C;}
            else if (ratio <= K4C)
                {b=B4C; m=M4C;}
            else if (ratio <= K5C)
                {b=B5C; m=M5C;}
            else if (ratio <= K6C)
                {b=B6C; m=M6C;}
            else if (ratio <= K7C)
                {b=B7C; m=M7C;}
            else if (ratio > K8C)
                {b=B8C; m=M8C;}
            break;
        }
    //unsigned long temp;
    temp = ((channel0 * b) - (channel1 * m));
    // do not allow negative lux value
    if (temp < 0) 
        temp = 0;
    // round lsb (2^(LUX_SCALE?1))
    temp += (1 << (LUX_SCALE-1));
    // strip off fractional portion
    lux = temp >> LUX_SCALE;
    return(lux);
}

/*---------------------------------------------------------*/
int tsl2561_open(struct inode *node,struct file *filp)
{
	printk("tsl2561 open!\n");
	TSL2561_Init(TIMING_402MS);
	return 0;
}


static long tsl2561_ioctl(struct file *file,
                            unsigned int cmd,
                            unsigned long arg)
{
    unsigned int Channel0_Val, Channel1_Val, Lux_Val;
	
    switch(cmd) {
            case GET_LUX: //读
                Channel0_Val = TSL2561_ChannelRead(TSL2561_DATA0_LOW, TSL2561_DATA0_HIGH);
                Channel1_Val = TSL2561_ChannelRead(TSL2561_DATA1_LOW, TSL2561_DATA1_HIGH);
                Lux_Val = CalculateLux(GAIN_1X, TIMING_402MS, Channel0_Val, Channel1_Val, T_PACKGE);
		printk("Lux_Val in kernel=%d\n", Lux_Val);
                copy_to_user((unsigned long *)arg, &Lux_Val, sizeof(Lux_Val));
                    
                break;
            default:
                return -EINVAL;
    }
    return 0;
}

static struct file_operations tsl2561_fops = {
    .owner = THIS_MODULE,
    .open = tsl2561_open,
    .unlocked_ioctl = tsl2561_ioctl
};


//miscdevice
static struct miscdevice tsl2561_miscdev = {
    .minor = MISC_DYNAMIC_MINOR, //自动分配次设备号
    .name = "tsl2561", //dev/tsl2561
    .fops = &tsl2561_fops
};

static int tsl2561_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;

	ret = misc_register(&tsl2561_miscdev);
	if (ret != 0)
	{
	    printk("tsl2561 register fail!\n");
	}
	else
	{
	    printk("tsl2561 register succeed!\n");
	}

	tsl2561_client = client;

        //i2c_smbus_write_byte_data(tsl2561_client, 0x80, 0x03);
	//i2c_smbus_write_byte_data(tsl2561_client, 0x81, 0x02);


	return 0;
}

static int tsl2561_remove(struct i2c_client *client) 
{
    //卸载混杂设备
    misc_deregister(&tsl2561_miscdev); 
    return 0; //成功返回0，失败返回负值
}


/*-------------------------------------------------------------------------*/

static const struct i2c_device_id tsl2561_id[] = {
	{"tsl2561", 0},//必须和i2c_board_info的type一致，匹配靠它进行
};


static struct i2c_driver tsl2561_driver = {
	.driver = {
		.name = "tsl2561",//不重要，匹配不靠它
		.owner = THIS_MODULE,
	},
	.probe = tsl2561_probe,
	.remove = tsl2561_remove,
	.id_table = tsl2561_id,
};


static int __init tsl2561_init(void)
{
	return i2c_add_driver(&tsl2561_driver);
}
module_init(tsl2561_init);

static void __exit tsl2561_exit(void)
{
	i2c_del_driver(&tsl2561_driver);
}
module_exit(tsl2561_exit);

MODULE_DESCRIPTION("Linux Driver for TSL2561");
MODULE_AUTHOR("Yu");
MODULE_LICENSE("GPL");
