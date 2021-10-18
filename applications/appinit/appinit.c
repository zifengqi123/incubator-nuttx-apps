#include <nuttx/nuttx.h>
#include <nuttx/compiler.h>

#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "../ledcontrol/led_ctrl_elf.h"

static void show_usage(FAR const char *progname)
{
  fprintf(stderr, "USAGE: %s [-u <url-path>] \n",
          progname);
  fprintf(stderr, "       %s -h\n", progname);
  fprintf(stderr, "Where:\n");
  fprintf(stderr, "\t<url-path>: The full path to the wget service.\n");
  fprintf(stderr, "\t-h: Print this usage information and exit.\n");
}


int main(int argc, FAR char *argv[])
{
    int ndx;
    bool hasUrl = false;
    FAR char *urlpath = NULL;

    if (argc < 2)
    {
      fprintf(stderr, "ERROR: Missing required arguments\n");
      show_usage(argv[0]);
      return EXIT_FAILURE;
    }

    ndx = 1;
    if (strcmp(argv[ndx], "-h") == 0)
    {
        show_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[ndx], "-u") == 0)
    {
        hasUrl = true;


        if (++ndx >= argc)
        {
            fprintf(stderr, "ERROR: Missing required <url-path>\n");
            show_usage(argv[0]);
            return EXIT_FAILURE;
        }


        urlpath = argv[ndx];
        
        int ret = elf_init_all(urlpath);
        printf("elf_init_all ret: %d\n", ret);

        if(ret != 0)
        {
          fprintf(stderr, "ERROR: application init fail\n");
          return EXIT_FAILURE;
        }

        printf("init success.\n");
    }
    else if(strcmp(argv[ndx], "-s") == 0)
    {
        printf("----start app---\n");

        int ret = 0;

        ret = elf_checkInitFlash();
        if(ret != 0)
        {
            printf("ERROR: Application not init.\n");
            return -1;
        }

        ret = elf_checkAppcfgFlash();
        if(ret != 0)
        {
            printf("ERROR: check application fail.\n");
            cJSON *json = elf_getInitParam();
            cJSON *ser = cJSON_GetObjectItem(json, "init_fs_ser");
            char* url = cJSON_GetStringValue(ser);

            ret = elf_init_appcfg(url);
            cJSON_free(json);
            if(ret != 0)
            {
                printf("ERROR: to init fail %d.\n", ret);
                return -2;
            }
        }

        elf_sys_execapp();
    }


    return 0;
}