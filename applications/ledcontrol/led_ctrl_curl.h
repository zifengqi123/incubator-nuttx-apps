#ifndef __LED_CTRL_CURL_H__
#define __LED_CTRL_CURL_H__

#include <stdio.h>
#include <stdlib.h>
#include "led_ctrl_def.h"
#include "netutils/cJSON.h"
#include "netutils/netlib.h"
#include "netutils/webclient.h"


int onLedStatus(FAR char * url, cJSON *in);

cJSON* getbackStatus();

#endif
