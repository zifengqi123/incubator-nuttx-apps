#include "led_ctrl_elf.h"


static int elf_wget_callback(FAR char **buffer, int offset, int datend,
                          FAR int *buflen, FAR void *arg)
{
    syslog(LOG_WARNING, "--wget_callback %d\n", *buflen);

    ssize_t written = write((int)((intptr_t)arg), &((*buffer)[offset]),
                            datend - offset);
    if (written == -1)
    {
        return -errno;
    }

    /* Revisit: Do we want to check and report short writes? */

    return 0;
}

int elf_doWget(FAR char * url, FAR char *fullpath)
{
    FAR char *buffer    = NULL;
    int fd = -1;
    int ret = 0;

    /* Open the local file for writing */
    syslog(LOG_WARNING, "--doWget url: %s, file: %s\n", url, fullpath);
    fd = open(fullpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        ret = -1;
        goto exit;
    }

    syslog(LOG_WARNING, "--doWget fd %d\n", fd);

    /* Allocate an I/O buffer */

    buffer = malloc(CONFIG_NSH_WGET_BUFF_SIZE);
    if (buffer == NULL)
    {
        goto errout;
    }

    syslog(LOG_WARNING, "--doWget todo webclient_perform\n");

    /* And perform the wget */
    ret = wget(url, buffer, CONFIG_NSH_WGET_BUFF_SIZE, elf_wget_callback, (FAR void *)((intptr_t)fd));
    syslog(LOG_WARNING, "--doWget todo webclient_perform ret : %d\n", ret);
    if (ret < 0)
    {
        errno = -ret;
        goto exit;
    }

    /* Free allocated resources */

exit:
    if (fd >= 0)
    {
        close(fd);
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return ret;

errout:
    ret = -1;
    goto exit;
}

uint32_t elf_fileSize(FAR char* file)
{
    struct stat buf;
    int ret = 0;
    uint32_t sz = 0;
    ret = stat(file, &buf);

    if(ret == 0) 
    {
        sz = buf.st_size;
    }
    else{
        sz = 0;
    }
    syslog(LOG_WARNING, "--%s size: %d\n", file, sz);

    return sz;

    // uint32_t sz = 0;
    // int fd = -1;
    // int ret = 0;

    // fd = open(file, O_RDONLY, 0644);

    // if (fd < 0)
    // {
    //     syslog(LOG_WARNING, "err: elf_fileSize fd: %d\n", fd);
    //     return 0;
    // }
    // // sz = lseek(fd, 0, SEEK_END);
    
    // uint8_t t;

    // for (; ;)
    // {
    //     ret = read(fd, &t, 1);
    //     if(ret > 0) {
    //         sz++;
    //     }
    //     else
    //     {
    //         break;
    //     }
    // }

    // if(fd >= 0)
    // {
    //     close(fd);
    // }

    // syslog(LOG_WARNING, "--%s size: %d\n", file, sz);
    
    // return sz;
}

int elf_writeFileToFlash(FAR char* file, uint32_t sz, uint32_t *addr)
{
    int ret = 0;
    int fd = -1;
    FAR uint8_t *buffer    = NULL;
    uint16_t crc16val = 0;

    uint32_t addr_t = *addr;

    usleep(200*100);
    fd = open(file, O_RDONLY, 0644);

    if (fd < 0)
    {
        ret = -1;
        syslog(LOG_WARNING, "elf_writeFileToFlash %s fail fd: %d\n", file, fd);
        goto exit;
    }

    buffer = (FAR uint8_t *)kmm_malloc(FLASH_BUFFER_SIZE);

    ret = up_progmem_write(addr_t, &sz, sizeof(uint32_t));
    if(ret < 0)
    {
        syslog(LOG_WARNING, "up_progmem_write size fail: %d\n", ret);
        goto exit;
    }
    addr_t += sizeof(uint32_t);

    int i = 0;
    for (i = 0; i < sz; )
    {
        memset(buffer, 0, FLASH_BUFFER_SIZE);
        ret = read(fd, buffer, FLASH_BUFFER_SIZE);
        if(ret < 0) 
        {
            ret = errno;
            break;
        }
        else if(ret == 0)
        {
            break;
        }
        else
        {
            crc16val = crc16part(buffer, ret, crc16val);
            ret = up_progmem_write(addr_t, buffer, FLASH_BUFFER_SIZE);  //注意这中间有空隙
            if(ret < 0)
            {
                syslog(LOG_WARNING, "up_progmem_write fail: %d\n", ret);
                goto exit;
            }

            i += FLASH_BUFFER_SIZE;
            addr_t += FLASH_BUFFER_SIZE;
        }
        usleep(200*100);
    }
    
    //写crc16
    ret = up_progmem_write(addr_t, &crc16val, sizeof(uint16_t));
    if(ret < 0)
    {
        syslog(LOG_WARNING, "up_progmem_write crc16 fail: %d\n", ret);
        goto exit;
    }
    addr_t += sizeof(uint16_t);
    *addr = addr_t;

exit:
    if (buffer != NULL)
    {
        free(buffer);
    }

    if (fd >= 0)
    {
        close(fd);
    }

    return ret;
}

int elf_writeAppCfgToFlash()
{
    int ret = 0;
    uint32_t addr = FLASH_ADDR_APP_S;

    uint32_t sz1 = elf_fileSize(APP_PATH);
    uint32_t sz2 = elf_fileSize(CFG_PATH);

    if(sz1 == 0 || sz2 == 0)
    {
        return -1;
    }

    up_progmem_eraseblock(FLASH_APP_SECTOR);    

    stm32_flash_writeprotect(FLASH_APP_SECTOR, false);

    //todo
    ret = elf_writeFileToFlash(APP_PATH, sz1, &addr);
    ret = elf_writeFileToFlash(CFG_PATH, sz2, &addr);

    
    stm32_flash_writeprotect(FLASH_APP_SECTOR, true);

    return ret;
}

int elf_writeInitParamToFlash()
{
    int ret = 0;
    uint32_t addr = FLASH_ADDR_INIT_S;
    uint32_t sz1 = elf_fileSize(INIT_PATH);

    if(sz1 == 0)
    {
        return -1;
    }

    up_progmem_eraseblock(FLASH_INIT_SECTOR);
    stm32_flash_writeprotect(FLASH_INIT_SECTOR, false);

    ret = elf_writeFileToFlash(INIT_PATH, sz1, &addr);

    stm32_flash_writeprotect(FLASH_INIT_SECTOR, true);
    return ret;
}


int elf_loadFlashToFile(uint32_t *addr, FAR char* file)
{
    int ret = 0;
    int fd = -1;
    FAR uint8_t *buffer    = NULL;
    uint32_t sz = 0;
    uint32_t addr_t = *addr;
    uint16_t crc16val = 0;
    int i = 0;
    int cnt = 0;
    int wret = 0;

    ret = 0;
    fd = -1;
    buffer = NULL;
    sz = 0;
    addr_t = *addr;
    crc16val = 0;
    i = 0;
    cnt = 0;
    wret = 0;

    usleep(200*100);
    ret = up_progmem_read(addr_t, &sz, sizeof(uint32_t));
    syslog(LOG_WARNING, "load Flash To File %x size : %d\n", addr_t, sz);
    if(ret < 0 || sz <= 0)
    {
        syslog(LOG_WARNING, "up_progmem_read size fail: %d\n", ret);
        ret = -107;
        goto exit;
    }
    addr_t += sizeof(uint32_t);

    while (fd < 0 && i < 10)
    {
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        i++;
    }
    syslog(LOG_WARNING, "open file %s ret: %d\n", file, fd);

    if (fd < 0)
    {
        ret = -1;
        goto exit;
    }

    buffer = (FAR uint8_t *)kmm_malloc(FLASH_BUFFER_SIZE);

    if(buffer == NULL)
    {
        syslog(LOG_WARNING, "buffer kmm_malloc ret NULL\n");
        ret = -2;
        goto exit;
    }

    for (cnt = 0; cnt < sz;)
    {
        memset(buffer, 0, FLASH_BUFFER_SIZE);

        ret = up_progmem_read(addr_t, buffer, FLASH_BUFFER_SIZE);
        // syslog(LOG_WARNING, "up_progmem_read ret: %d\n", ret);
        if(ret > 0)
        {
            cnt += ret;
            addr_t += ret;
            if(cnt < sz) 
            {
                crc16val = crc16part(buffer, ret, crc16val);
                wret = write(fd, buffer, ret);
                if(wret != ret)
                {
                    syslog(LOG_WARNING, "--err write file : %d != %d\n", wret, ret);

                    ret = -111;
                    goto exit;
                }
            }
            else 
            {
                crc16val = crc16part(buffer, ret - (cnt-sz), crc16val);
                wret = write(fd, buffer, ret - (cnt-sz));
                if(wret != ret - (cnt-sz))
                {
                    syslog(LOG_WARNING, "--err write file : %d != %d\n", wret, ret - (cnt-sz));
                    ret = -112;
                    goto exit;
                }
            }
        }
        else
        {
            ret = -113;
            goto exit;
        }
        usleep(200*100);

    }

    uint16_t tcrc = 0;
    ret = up_progmem_read(addr_t, &tcrc, sizeof(uint16_t));
    if(ret < 0)
    {
        syslog(LOG_WARNING, "up_progmem_read crc16 fail: %d\n", ret);
        goto exit;
    }
    addr_t += sizeof(uint16_t);
    *addr = addr_t;

    syslog(LOG_WARNING, "file crc %d ? %d\n", crc16val, tcrc);
    if(crc16val == tcrc) 
    {
        ret = 0;
    }
    else
    {
        ret = -119;
    }

    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }

    cnt = elf_fileSize(file);
    syslog(LOG_WARNING, "file size %d ? %d\n", cnt, sz);

    if(cnt != sz) 
    {
        ret = -120;
    }
    
exit:
    if (buffer != NULL)
    {
        free(buffer);
    }

    if (fd >= 0)
    {
        close(fd);
    }

    return ret;
}

int elf_checkAppcfgFlash()
{
    int ret = 0;
    uint32_t addr = FLASH_ADDR_APP_S;

    for (size_t i = 0; i < 3; i++)
    {
        elf_sys_rmfile(RM_APP_CMD);
        ret = elf_loadFlashToFile(&addr, APP_PATH);

        if (ret != 0) 
        {
            continue;
        }

        elf_sys_rmfile(RM_CFG_CMD);
        ret = elf_loadFlashToFile(&addr, CFG_PATH);

        if(ret == 0) 
        {
            break;
        }
    }

    syslog(LOG_WARNING, ":elf_checkAppcfgFlash ret: %d\n", ret);

    return ret;
}

int elf_checkInitFlash()
{
    uint32_t addr = FLASH_ADDR_INIT_S;
    int ret = 0;

    for (size_t i = 0; i < 3; i++)
    {
        elf_sys_rmfile(RM_INIT_CMD);
        ret = elf_loadFlashToFile(&addr, INIT_PATH);
       
        if(ret == 0) 
        {
            break;
        }
    }
    
    syslog(LOG_WARNING, ":elf_checkInitFlash ret: %d\n", ret);
    return ret;
}



int elf_wget_application(FAR char* url)
{
    elf_sys_rmfile(RM_APP_CMD);
    return elf_doWget(url, APP_PATH);
}

int elf_wget_config(FAR char* url)
{
    elf_sys_rmfile(RM_CFG_CMD);
    return elf_doWget(url, CFG_PATH);
}

int elf_wget_initparam(FAR char* url)
{
    elf_sys_rmfile(RM_INIT_CMD);
    return elf_doWget(url, INIT_PATH);
}

int elf_init_appcfg(FAR char* url)
{
    int ret = 0;
    char appurl[128] = {0};
    char cfgurl[128] = {0};
    
    sprintf(appurl, "%s/%s", url, ELF_APP_NAME);
    sprintf(cfgurl, "%s/%s", url, ELF_CFG_NAME);

    ret = elf_wget_application(appurl);
    if(ret < 0) return -1;

    ret = elf_wget_config(cfgurl);
    if(ret < 0) return -2;

    ret = elf_writeAppCfgToFlash();
    if(ret < 0) return -3;

    ret = elf_checkAppcfgFlash();
    if(ret != 0) return -4;

    return 0;
}

int elf_init_param(FAR char* url)
{
    int ret = 0;
    char initurl[128] = {0};
    
    sprintf(initurl, "%s/%s", url, ELF_INIT_NAME);

    ret = elf_wget_initparam(initurl);
    if(ret < 0) return -5;

    ret = elf_writeInitParamToFlash();
    if(ret < 0) return -6;

    ret = elf_checkInitFlash();
    if(ret != 0) return -7;

    return 0;
}

int elf_init_all(FAR char* url)
{
    int ret = 0;
    
    ret = elf_init_appcfg(url);
    if(ret != 0) return ret;

    ret = elf_init_param(url);
    if(ret != 0) return ret;

    return 0;
}

cJSON* elf_getFileJson(FAR char* file)
{
    uint32_t sz = elf_fileSize(file);
    int fd = -1;
    int ret = 0;
    cJSON* json = NULL;

    fd = open(file, O_RDONLY, 0644);

    if (fd < 0)
    {
        syslog(LOG_WARNING, "elf_getFileJson %s open failed.\n", file);
        return NULL;
    }

    char *buffer = (FAR uint8_t *)kmm_malloc(sz+1);

    memset(buffer, 0, sz+1);
    ret = read(fd, buffer, sz);
    syslog(LOG_WARNING, "elf_getFileJson buff: %s\n", buffer);

    if(ret == sz)
    {
        json = cJSON_Parse(buffer);
    } 

    free(buffer);
    close(fd);

    return json;
}

cJSON* elf_getInitParam()
{
    return elf_getFileJson(INIT_PATH);
}

cJSON* elf_getConfig()
{
    return elf_getFileJson(CFG_PATH);
}

void elf_sys_execapp()
{
    syslog(LOG_WARNING, "start application...\n");
    system(EXEC_APP_CMD);
}
void elf_sys_reboot()
{
    syslog(LOG_WARNING, "reboot system...\n");
    system(EXEC_REBOOT_CMD);
}

/**
  * @brief 版本号比较那个更新
  * @param pszStr1 待比较的版本号
  * @param pszStr2 待比较的版本号
  * @retval >0:pszStr1更新
  			=0:相同
  			<0:pszStr2更新
  		例：	"2.0.10.6" 与 "2.0.3.9"相比较的返回结果为7
  			"2.0.3.9" 与 "2.0.10.6"相比较的返回结果为-7
 **/
int edition_compare(const char* pszStr1, const char* pszStr2)
{
	if (pszStr1 == NULL || pszStr2 == NULL) {
		return 0;
	}
	int nCurPos = 0, nCapPos=-1;
	const char* pszTmp1 = pszStr1;
	const char* pszTmp2 = pszStr2;
	while ((*pszTmp1 != '\0') && (*pszTmp2 != '\0') && (*pszTmp1 == *pszTmp2)) {	
		nCurPos++;									//找到第一个处不相同出现的位置
		pszTmp1++;
		pszTmp2++;
		if (*pszTmp1 == '.') {
			nCapPos = nCurPos;						//记录最近的‘.’的位置
		}
	}
	if (*pszTmp1 == '\0' && *pszTmp2 == '\0') { // 两个字符串相等
		return 0;
	} else if(*pszTmp1 == '\0'){
		return -1;
	} else if(*pszTmp2 == '\0'){
		return 1;
	}else{ // 两个字符串不相等，比较大小
		pszTmp1 = pszStr1 + nCapPos + 1;
		pszTmp2 = pszStr2 + nCapPos + 1;

		int pszNub1=strtol(pszTmp1,NULL,10);
		int pszNub2=strtol(pszTmp2,NULL,10);

		return (pszNub1 - pszNub2);
	}
}

void elf_sys_rmfile(FAR char* cmd)
{
    system(cmd);
}