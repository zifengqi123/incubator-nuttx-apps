#ifndef ___LED_CTRL_DEFINE_H__
#define ___LED_CTRL_DEFINE_H__
#include <debug.h>

struct LED_STATUS
{
    int index;
    bool isOn;
    double level;
    bool isChange;
    int state;
};

#if defined(CONFIG_DEV_GPIO)

#include <nuttx/ioexpander/gpio.h>


#define GPIO_LED1      "/dev/gpout0"
#define GPIO_LED2      "/dev/gpout1"
#define GPIO_LED3      "/dev/gpout2"
#define GPIO_LED4      "/dev/gpout3"
#define GPIO_LED5      "/dev/gpout4"
#define GPIO_LED6      "/dev/gpout5"
#define GPIO_LED7      "/dev/gpout6"
#define GPIO_LED8      "/dev/gpout7"

#endif


long getTimestamp();



// 1. 灌装程序，用来出厂灌装SN、默认Wget地址、程序、配置参数至 flash。
// 2. 启动程序，从flash中加载程序并运行，做程序校验及异常处理。
// 3. 程序wget自更新。

#endif
