#include "../ledcontrol/led_ctrl_elf.h"

int main(int argc, FAR char *argv[])
{
    syslog(LOG_WARNING, "----start user define main---\n");

    int ret = 0;

    elf_sys_rmfile(RM_APP_CMD);
    elf_sys_rmfile(RM_CFG_CMD);
    elf_sys_rmfile(RM_INIT_CMD);
    
    ret = elf_checkInitFlash();
    if(ret != 0)
    {
        syslog(LOG_ERR, "ERROR: Application not init.\n");
        return -1;
    }

    ret = elf_checkAppcfgFlash();
    if(ret != 0)
    {
        syslog(LOG_ERR, "ERROR: check application fail.\n");
        cJSON *json = elf_getInitParam();
        cJSON *ser = cJSON_GetObjectItem(json, "init_fs_ser");
        char* url = cJSON_GetStringValue(ser);

        ret = elf_init_appcfg(url);
        cJSON_free(json);
        if(ret != 0)
        {
            syslog(LOG_ERR, "ERROR: to init fail %d.\n", ret);
        }
        elf_sys_reboot();
    }
    else
    {
        elf_sys_execapp();
    }

    return 0;
}