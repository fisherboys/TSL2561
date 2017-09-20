
/******************** (C) COPYRIGHT 2017 YU **************************
 * FILE NAME  		：TSL2561
 * DESCRIPTION    	：LINUX DRIVER FOR TSL2561(GY2561)
 * KERNEL			：LINUX 3.0.35
 * SYSTEM	  		：ANDROID 4.4.2
 * VERSION	  		：1.0
 * RELEASE DATE	  	：2017/09/20
 * PLATFORM  		：IMX6Q
 * AUTHOR    		：YU
*********************************************************************/
#define TSL2561_ADDR0 0x29 // address with '0' shorted on board
#define TSL2561_ADDR  0x39 // default address
#define TSL2561_ADDR1 0x49 // address with '1' shorted on board

// TSL2561 registers

#define TSL2561_CMD           0x80
#define TSL2561_TIMING        0x81
#define	TSL2561_REG_ID        0x8A
#define	TSL2561_DATA0_LOW     0x8C
#define TSL2561_DATA0_HIGH    0x8D
#define	TSL2561_DATA1_LOW     0x8E
#define TSL2561_DATA1_HIGH    0x8F
//Control Register Value

#define TSL2561_POWER_ON      0x03
#define TSL2561_POWER_DOWN    0x00

//Timing Register Value.Set integration time
                                      //set integration time
#define  TIMING_13MS         0x00     //integration time 13.7ms
#define  TIMING_101MS        0x01     //integration time 101ms
#define  TIMING_402MS        0x02     //integration time 402ms





/*********************************************
*  Parameter calculation of intensity        *
**********************************************/
#define LUX_SCALE     14     //scale by 2^14
#define RATIO_SCALE   9      //scale ratio by 2^9
//package type (T or CS)
#define T_PACKGE     0
#define CS_PACKGE    1
// Arguments: unsigned int iGain ? gain, where 0:1X, 1:16X
#define GAIN_1X       0
#define GAIN_16X      1
//Integration time scaling factors
#define CH_SCALE      10     //scale channel valuse by2^10
#define CHSCALE_TINT0 0x7517 //322/11*2^CH_SCALE
#define CHSCALE_TINT1 0x0fe7 //322/81*2^CH_SCALE
//T,FN,andCLPackage cofficients
#define K1T 0x0040 // 0.125 * 2^RATIO_SCALE
#define B1T 0x01f2 // 0.0304 * 2^LUX_SCALE
#define M1T 0x01be // 0.0272 * 2^LUX_SCALE
#define K2T 0x0080 // 0.250 * 2^RATIO_SCALE
#define B2T 0x0214 // 0.0325 * 2^LUX_SCALE
#define M2T 0x02d1 // 0.0440 * 2^LUX_SCALE
#define K3T 0x00c0 // 0.375 * 2^RATIO_SCALE
#define B3T 0x023f // 0.0351 * 2^LUX_SCALE
#define M3T 0x037b // 0.0544 * 2^LUX_SCALE
#define K4T 0x0100 // 0.50 * 2^RATIO_SCALE
#define B4T 0x0270 // 0.0381 * 2^LUX_SCALE
#define M4T 0x03fe // 0.0624 * 2^LUX_SCALE
#define K5T 0x0138 // 0.61 * 2^RATIO_SCALE
#define B5T 0x016f // 0.0224 * 2^LUX_SCALE
#define M5T 0x01fc // 0.0310 * 2^LUX_SCALE
#define K6T 0x019a // 0.80 * 2^RATIO_SCALE
#define B6T 0x00d2 // 0.0128 * 2^LUX_SCALE
#define M6T 0x00fb // 0.0153 * 2^LUX_SCALE
#define K7T 0x029a // 1.3 * 2^RATIO_SCALE
#define B7T 0x0018 // 0.00146 * 2^LUX_SCALE
#define M7T 0x0012 // 0.00112 * 2^LUX_SCALE
#define K8T 0x029a // 1.3 * 2^RATIO_SCALE
#define B8T 0x0000 // 0.000 * 2^LUX_SCALE
#define M8T 0x0000 // 0.000 * 2^LUX_SCALE
#define K1C 0x0043 // 0.130 * 2^RATIO_SCALE
#define B1C 0x0204 // 0.0315 * 2^LUX_SCALE
#define M1C 0x01ad // 0.0262 * 2^LUX_SCALE
#define K2C 0x0085 // 0.260 * 2^RATIO_SCALE
#define B2C 0x0228 // 0.0337 * 2^LUX_SCALE
#define M2C 0x02c1 // 0.0430 * 2^LUX_SCALE
#define K3C 0x00c8 // 0.390 * 2^RATIO_SCALE
#define B3C 0x0253 // 0.0363 * 2^LUX_SCALE
#define M3C 0x0363 // 0.0529 * 2^LUX_SCALE
#define K4C 0x010a // 0.520 * 2^RATIO_SCALE
#define B4C 0x0282 // 0.0392 * 2^LUX_SCALE
#define M4C 0x03df // 0.0605 * 2^LUX_SCALE
#define K5C 0x014d // 0.65 * 2^RATIO_SCALE
#define B5C 0x0177 // 0.0229 * 2^LUX_SCALE
#define M5C 0x01dd // 0.0291 * 2^LUX_SCALE
#define K6C 0x019a // 0.80 * 2^RATIO_SCALE
#define B6C 0x0101 // 0.0157 * 2^LUX_SCALE
#define M6C 0x0127 // 0.0180 * 2^LUX_SCALE
#define K7C 0x029a // 1.3 * 2^RATIO_SCALE
#define B7C 0x0037 // 0.00338 * 2^LUX_SCALE
#define M7C 0x002b // 0.00260 * 2^LUX_SCALE
#define K8C 0x029a // 1.3 * 2^RATIO_SCALE
#define B8C 0x0000 // 0.000 * 2^LUX_SCALE
#define M8C 0x0000 // 0.000 * 2^LUX_SCALE
/**************************************************/
unsigned int TSL2561_Read(unsigned char command);
void TSL2561_PowerOn(void);
void TSL2561_PowerDown(void);
void TSL2561_TimingSet(unsigned char TIME);
void TSL2561_Init(unsigned char TIME);
unsigned int TSL2561_ChannelRead(unsigned char Low_Addr, unsigned char High_Addr);
unsigned int CalculateLux(unsigned int iGain, 
	                  unsigned int tInt,
 			  unsigned int ch0,
                          unsigned int ch1, 
			  int iType);
