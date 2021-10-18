#include "../ledcontrol/led_ctrl_elf.h"

int main(int argc, FAR char *argv[])
{
    printf("----start user define main---\n");

    int ret = 0;
    int start = 0;

    elf_sys_rmfile(RM_APP_CMD);
    elf_sys_rmfile(RM_CFG_CMD);
    elf_sys_rmfile(RM_INIT_CMD);
    
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
        }
    }
    else
    {
        cJSON *json = elf_getConfig();
        cJSON *flag = cJSON_GetObjectItem(json, "not_autos");
        if(flag != NULL)
        {
            start = flag->valueint;
            
        }
        
        if(start != 1)
        {
            elf_sys_execapp();
        }
        
    }

    return 0;
}