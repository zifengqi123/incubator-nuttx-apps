#ifndef __LED_CTRL_PTHREAD_H__
#define __LED_CTRL_PTHREAD_H__


#include <fcntl.h>
#include <nuttx/timers/timer.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include "led_ctrl_def.h"
#include "led_ctrl_curl.h"


struct timespec getRealTime();
void diffTimespec(struct timespec t1, struct timespec t2);
void timer_printf(char * tt);
void pth_usleep(uint32_t utm);

#if defined(CONFIG_DEV_GPIO)

void start_gpio_thread();
void stop_gpio_thread();

struct LED_STATUS *getNewLedStatusPtr();
struct LED_STATUS *getLedstatusPtr();

#endif

#endif
