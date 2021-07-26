#include "led_ctrl_pthread.h"


static bool pth_flag[8];
static float ledPer = 0.51;
static int all_t = 2 * 1000;
static pthread_t p_ledctrl[8];

static struct LED_STATUS ledstatus[8];
static struct LED_STATUS newLedStatus[8];

struct timespec getRealTime()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts)) {
        perror("clock_getime ");
    }
    return ts;
}

struct LED_STATUS *getNewLedStatusPtr()
{
    return newLedStatus;
}

struct LED_STATUS *getLedstatusPtr()
{
    return ledstatus;
}

void diffTimespec(struct timespec t1, struct timespec t2)
{
    printf("time start with %lu s, %ld ns\n", t1.tv_sec, t1.tv_nsec);
    printf("time  end  with %lu s, %ld ns\n", t2.tv_sec, t2.tv_nsec);

    uint32_t t = t2.tv_sec - t1.tv_sec;

    if (t > 0)
    {
        t = t*1000000 + t2.tv_nsec/1000;
        t = t - t1.tv_nsec/1000;
    }
    else 
    {
        t =  (t2.tv_nsec - t1.tv_nsec)/1000;
    }

    printf("time diff: %d us\n", t);

}

void timer_printf(char * tt)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts)) {
        perror("clock_getime ");
    }
    // printf("%s time with %lu s, %ld ns\n", tt, ts.tv_sec, ts.tv_nsec);
}


// sem_t sem_block;
// const char* devname = "/dev/timer2";
// int flag = 0;

// static void timer_sighandler(int signo, FAR siginfo_t *siginfo,
//                              FAR void *context)
// {
//     if (signo == 17)
//     {
//         flag ++;
//         if (flag > 1)
//         {
//             sem_post(&sem_block);
//         }
//     }
// }

// int timer_usleep(uint32_t tmus)
// {
//     #define CONFIG_EXAMPLES_TIMER_SIGNO 17
//     struct timer_notify_s notify;
//     struct sigaction act;
//     int ret = 0;
//     int fd = -1;
//     uint32_t t1 = 0, t2 = 0, t3 = 0;

//     /* Open the timer device */

//     if (sem_init(&sem_block, 0, 0)) {
//         printf("sem_init error!\n");
//         return EXIT_FAILURE;
//     }

//     flag = 0;

//     // printf("Open %s\n", devname);

//     fd = open(devname, O_RDONLY);
//     if (fd < 0)
//       {
//         fprintf(stderr, "ERROR: Failed to open %s: %d\n",
//                 devname, errno);
//         return EXIT_FAILURE;
//       }

//     // interval setting needed with unit us
//     ret = ioctl(fd, TCIOC_SETTIMEOUT, tmus);

//     /* Attach a signal handler to catch the notifications.  NOTE that using
//      * signal handler is very slow.  A much more efficient thing to do is to
//      * create a separate pthread that waits on sigwaitinfo() for timer events.
//      * Much less overhead in that case.
//      */


//     act.sa_sigaction = timer_sighandler;
//     act.sa_flags     = SA_SIGINFO;

//     sigfillset(&act.sa_mask);
//     sigdelset(&act.sa_mask, CONFIG_EXAMPLES_TIMER_SIGNO);

//     ret = sigaction(CONFIG_EXAMPLES_TIMER_SIGNO, &act, NULL);
//     if (ret != OK)
//     {
//         fprintf(stderr, "ERROR: Fsigaction failed: %d\n", errno);
//         close(fd);
//         return EXIT_FAILURE;
//     }
//     /* Register a callback for notifications using the configured signal.
//      *
//      * NOTE: If no callback is attached, the timer stop at the first interrupt.
//      */

//     // printf("Attach timer handler\n");

//     notify.pid   = getpid();

//     notify.event.sigev_notify = SIGEV_SIGNAL;
//     notify.event.sigev_signo  = CONFIG_EXAMPLES_TIMER_SIGNO;
//     notify.event.sigev_value.sival_ptr = NULL;

//     ret = ioctl(fd, TCIOC_NOTIFICATION, (unsigned long)((uintptr_t)&notify));
//     if (ret < 0)
//       {
//         fprintf(stderr, "ERROR: Failed to set the timer handler: %d\n", errno);
//         close(fd);
//         return EXIT_FAILURE;
//       }

//     /* Start the timer */
//     // printf("Start the timer\n");
//     t1 = getRealTime();

//     // reset the timer counter before start
//     ret = ioctl(fd, TCIOC_START, 0);
//     if (ret < 0)
//     {
//         fprintf(stderr, "ERROR: Failed to start the timer: %d\n", errno);
//         close(fd);
//         return EXIT_FAILURE;
//     }

//     sem_wait(&sem_block);

//     // wait for stop
//     // int sem_ret;
//     // while ((sem_ret = sem_wait(&sem_block)) != 0) {
//     //     // printf("sem_wait return with code %d, info: %s\n", sem_ret, strerror(sem_ret));
//     // }


//     /* Stop the timer */
//     t3 = getRealTime();
//     // printf("Stop the timer\n");
//     // printf("time test, timer duration: %ld us\n",  t3 - t1);

//     ret = ioctl(fd, TCIOC_STOP, 0);
//     if (ret < 0)
//     {
//         fprintf(stderr, "ERROR: Failed to stop the timer: %d\n", errno);
//         close(fd);
//         return EXIT_FAILURE;
//     }

//     /* Detach the signal handler */

//     act.sa_handler = SIG_DFL;
//     sigaction(CONFIG_EXAMPLES_TIMER_SIGNO, &act, NULL);
//     close(fd);
//     sem_destroy(&sem_block);
//     // printf("Finished\n");

//     return 0;
// }


void pth_usleep(uint32_t utm)
{
    // timer_usleep(utm);
   
    usleep(utm);
    
    // struct timespec ts;
    // if (clock_gettime(CLOCK_REALTIME, &ts)) {
    //     perror("clock_getime ");
    // }

    // uint64_t tx = utm * 1000;
    // struct timespec tse;
    // tse.tv_sec = tx/(1000*1000*1000);
    // tse.tv_nsec = tx%(1000*1000*1000);

    // nanosleep(&tse, NULL);
    
    // uint64_t t = utm*400;
    // while (t--);
    
}

void getDelayTime(int level, int *h, int *l) 
{
    float t = level;
    t = t/100.0;
    float per = ledPer + (0.85-ledPer) * t;

    *h = all_t * per;
    *l = all_t - (*h);
}

#if defined(CONFIG_DEV_GPIO)
void pth_led1_callback()
{
    struct LED_STATUS *new = newLedStatus;
    struct LED_STATUS *sts = ledstatus;
    
    int fd = open(GPIO_LED1, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED1, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[0])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
    
}

void pth_led2_callback()
{
    struct LED_STATUS *new = newLedStatus + 1;
    struct LED_STATUS *sts = ledstatus + 1;
    
    int fd = open(GPIO_LED2, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED2, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[1])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
}

void pth_led3_callback()
{
    struct LED_STATUS *new = newLedStatus + 2;
    struct LED_STATUS *sts = ledstatus + 2;
    
    int fd = open(GPIO_LED3, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED3, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[2])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
}

void pth_led4_callback()
{
    struct LED_STATUS *new = newLedStatus + 3;
    struct LED_STATUS *sts = ledstatus + 3;
    
    int fd = open(GPIO_LED4, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED4, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[3])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
}

void pth_led5_callback()
{
    struct LED_STATUS *new = newLedStatus + 4;
    struct LED_STATUS *sts = ledstatus + 4;
    
    int fd = open(GPIO_LED5, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED5, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[4])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
}

void pth_led6_callback()
{
    struct LED_STATUS *new = newLedStatus + 5;
    struct LED_STATUS *sts = ledstatus + 5;
    
    int fd = open(GPIO_LED6, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED6, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[5])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
}

void pth_led7_callback()
{
    struct LED_STATUS *new = newLedStatus + 6;
    struct LED_STATUS *sts = ledstatus + 6;
    
    int fd = open(GPIO_LED7, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED7, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[6])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
}

void pth_led8_callback()
{
    struct LED_STATUS *new = newLedStatus + 7;
    struct LED_STATUS *sts = ledstatus + 7;
    
    int fd = open(GPIO_LED8, O_RDWR);
    if (fd < 0)
    {
        int errcode = errno;
        sts->state = errcode;
        fprintf(stderr, "ERROR: Failed to open %s: %d\n", GPIO_LED8, errcode);
        return ;
    }

    float per = 0;
    int h_t = all_t *  per;
    int l_t = all_t - h_t;

    while (pth_flag[7])
    {
        if (!new->isOn)
        {
            sts->isOn = new->isOn;
            sts->level = new->level;
            new->isChange = false;

            ioctl(fd, GPIOC_WRITE, (unsigned long)false);
            pth_usleep(all_t);
        }
        else
        {
            if (new->isChange)
            {
                getDelayTime(new->level, &h_t, &l_t);

                new->isChange = false;

                sts->isOn = new->isOn;
                sts->level = new->level;
                printf("LED %d is change status: %d, %d\n", new->index, new->isOn, new->level);
            }

            ioctl(fd, GPIOC_WRITE, (unsigned long)true);
            pth_usleep(h_t);
            if (l_t > 0)
            {
                ioctl(fd, GPIOC_WRITE, (unsigned long)false);
                pth_usleep(l_t);
            }
        }
    }

    close(fd);
}



void start_gpio_thread()
{
    int ret = 0;

#if defined(CONFIG_DEV_GPIO)


    pth_flag[0] = true;
    ret = pthread_create(&p_ledctrl[0], NULL, pth_led1_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 1 fail!!!\n");
        return ret;
    }
    pth_flag[1] = true;
    ret = pthread_create(&p_ledctrl[1], NULL, pth_led2_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 2 fail!!!\n");
        return ret;
    }
    pth_flag[2] = true;
    ret = pthread_create(&p_ledctrl[2], NULL, pth_led3_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 3 fail!!!\n");
        return ret;
    }
    pth_flag[3] = true;
    ret = pthread_create(&p_ledctrl[3], NULL, pth_led4_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 4 fail!!!\n");
        return ret;
    }
    pth_flag[4] = true;
    ret = pthread_create(&p_ledctrl[4], NULL, pth_led5_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 5 fail!!!\n");
        return ret;
    }
    pth_flag[5] = true;
    ret = pthread_create(&p_ledctrl[5], NULL, pth_led6_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 6 fail!!!\n");
        return ret;
    }
    pth_flag[6] = true;
    ret = pthread_create(&p_ledctrl[6], NULL, pth_led7_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 7 fail!!!\n");
        return ret;
    }
    pth_flag[7] = true;
    ret = pthread_create(&p_ledctrl[7], NULL, pth_led8_callback, NULL);
    if (ret != 0)
    {
        printf("thread create 8 fail!!!\n");
        return ret;
    }
    
#endif
}


void stop_gpio_thread()
{
    for (size_t i = 0; i < 8; i++)
    {
        pth_flag[i] = false;
    }
    
    for (size_t i = 0; i < 8; i++)
    {
        pthread_join(p_ledctrl[i], NULL);
    }
}



#endif