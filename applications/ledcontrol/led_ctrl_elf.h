#ifndef __LED_CTRL_ELF_H___
#define __LED_CTRL_ELF_H___
#include <nuttx/nuttx.h>
#include <nuttx/compiler.h>

#include <sys/stat.h>    /* Needed for open */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <fcntl.h>       /* Needed for open */
#include <dirent.h>
#include <libgen.h>      /* Needed for basename */
#include <assert.h>
#include <errno.h>
#include <debug.h>
#include "netutils/webclient.h"
#include <nuttx/mtd/mtd.h>
#include <nuttx/kmalloc.h>
#include <crc16.h>
#include <sys/statfs.h>

#include "netutils/cJSON.h"

#define ELF_APP_NAME    "ledcontrol"
#define ELF_CFG_NAME    "config"
#define ELF_INIT_NAME   "initparam"


#define APP_PATH        "/mnt/nxffs/ledcontrol"
#define CFG_PATH        "/mnt/nxffs/config"
#define INIT_PATH       "/mnt/nxffs/initparam"

#define EXEC_APP_CMD    "/mnt/nxffs/ledcontrol &"
#define EXEC_REBOOT_CMD    "reboot"

#define RM_APP_CMD      "rm -r /mnt/nxffs/ledcontrol"
#define RM_CFG_CMD      "rm -r /mnt/nxffs/config"
#define RM_INIT_CMD     "rm -r /mnt/nxffs/initparam"

#define FLASH_APP_SECTOR    1
#define FLASH_ADDR_APP_S    0x08004000

#define FLASH_INIT_SECTOR    2
#define FLASH_ADDR_INIT_S    0x08008000


#define FLASH_BUFFER_SIZE     16


int elf_init_all(FAR char* url);
int elf_init_appcfg(FAR char* url);

int elf_wget_application(FAR char* url);
int elf_wget_config(FAR char* url);
int elf_wget_initparam(FAR char* url);

int elf_writeAppCfgToFlash();
int elf_writeInitParamToFlash();

int elf_checkAppcfgFlash();
int elf_checkInitFlash();

cJSON* elf_getInitParam();
cJSON* elf_getConfig();

void elf_sys_execapp();
void elf_sys_reboot();
void elf_sys_rmfile(FAR char* cmd);

int edition_compare(const char* pszStr1, const char* pszStr2);


#endif
