# TSL2561
TSL2561(GY2561) linux/android driver for IMX6 platform

### 更改板级文件
在kernel_imx/arch/arm/mach-mx6/board-mx6q_sabresd.c文件中静态声明一个I2C设备:
```C
static struct i2c_board_info mxc_i2c2_board_info2[] __initdata = {
    {
        I2C_BOARD_INFO("tsl2561", 0x39),
    },
}
```
在mx6_sabresd_board_init()函数中，向总线注册I2C设备信息：
```C
i2c_register_board_info(2, mxc_i2c2_board_info2, ARRAY_SIZE(mxc_i2c2_board_info2));
```
重新编译内核，烧写开发板<br>
### 编译加载驱动
下载源码，生成动态加载模块tsl2561.ko,在源码路径下执行以下命令：
```shell
make
```
将tsl2561.ko复制到开发板/data/local/tmp目录并执行以下命令:
```shell
insmod tsl2561.ko
```
### 测试应用程序
```C
//文件名：tsl2561_app.c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define TSL_MAGIC 'T'       //定义幻数
#define GET_LUX _IOW('T', 0, int)

int main()
{
    int fd;
    int Lux_Val;
    
    fd = open("/dev/tsl2561", ORDWR);       //打开光感模块 
    if (fd < 0)
        printf("open tsl2561 fail!\n");
    ioctl(fd, GET_LUX, (unsigned long) &Lux_Val);       
 
    printf("Lux_Val=%d\n", Lux_Val);        //打印光强值
    
    return 0；
}
```
执行以下命令生成可执行文件tsl2561_app：
```shell
arm-linux-gcc -static tsl2561_app.c -o tsl2561_app
```
在开发板中执行以下命令即可打印光感传感器的光强值：
```shell
./tsl2561_app
```
