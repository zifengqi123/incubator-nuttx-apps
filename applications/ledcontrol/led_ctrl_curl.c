#include "led_ctrl_curl.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

static char post_buff[1024] = {0};
static char buffer[1024] = {0};
#define URL_MAXSIZE 1024
static FAR char *g_json_buff = NULL;
static int g_json_bufflen = 0;
static cJSON *backStatus = NULL;

/****************************************************************************
 * Name: wgetjson_callback
 ****************************************************************************/

static int wgetjson_callback(FAR char **buffer, int offset, int datend,
                             FAR int *buflen, FAR void *arg)
{
  FAR char *new_json_buff;
  int len = datend - offset;
  int org = len;

  if (len <= 0)
  {
    return 0;
  }

  if (!g_json_buff)
  {
    g_json_buff = malloc(len + 1);
    memcpy(g_json_buff, &((*buffer)[offset]), len);
    g_json_buff[len] = 0;
    g_json_bufflen = len;
  }
  else
  {
    if (g_json_bufflen >= URL_MAXSIZE)
    {
      g_json_bufflen += org;
      return 0;
    }

    if (g_json_bufflen + len > URL_MAXSIZE)
    {
      len = URL_MAXSIZE - g_json_bufflen;
    }

    new_json_buff = (FAR char *)realloc(g_json_buff,
                                        g_json_bufflen + len + 1);
    if (new_json_buff)
    {
      g_json_buff = new_json_buff;
      memcpy(&g_json_buff[g_json_bufflen - 1], &((*buffer)[offset]),
             len);
      g_json_buff[g_json_bufflen + len] = 0;
      g_json_bufflen += org;
    }
  }

  return 0;
}

/****************************************************************************
 * Name: wgetjson_json_release
 ****************************************************************************/

static void wgetjson_json_release(void)
{
  if (g_json_buff)
  {
    free(g_json_buff);
    g_json_buff = NULL;
  }

  g_json_bufflen = 0;
}

int onLedStatus(FAR char * url, cJSON *in)
{
  int buffer_len = 512;
  // char *url = URL_STATUS;
  int ret = -1;
  int post_buff_len = 0;

  webclient_sink_callback_t wget_cb = wgetjson_callback;

  buffer_len = 512 * 2;
  backStatus = NULL;

  memset(post_buff, 0, 1024);
  char *pin = cJSON_Print(in);
  sprintf(post_buff, "%s", pin);
  if(pin != NULL)
  {
    free(pin);
  }
  
  memset(buffer, 0, 1024);
  wgetjson_json_release();

  struct webclient_context ctx;
  webclient_set_defaults(&ctx);
  ctx.buffer = buffer;
  ctx.buflen = buffer_len;
  ctx.sink_callback = wget_cb;
  ctx.sink_callback_arg = NULL;

  post_buff_len = strlen(post_buff);

  if (post_buff_len > 0)
  {
    const char *header = "Content-Type: "
                         "application/x-www-form-urlencoded";
    // const char *header = "Content-Type: "
    //                      "application/json";

    ctx.method = "POST";
    ctx.url = url;
    ctx.headers = &header;
    ctx.nheaders = 1;
    webclient_set_static_body(&ctx, post_buff, strlen(post_buff));
    ret = webclient_perform(&ctx);
  }

  if (ret < 0)
  {
    printf("get json size: %d, ret: %d\n", g_json_bufflen, ret);
    if (g_json_bufflen > 0)
    {
      printf("get json --: %s\n", g_json_buff);
    }
  }
  else
  {
    if (g_json_bufflen > 0)
    {
      printf("get json ++: %s\n", g_json_buff);
      backStatus = cJSON_Parse(g_json_buff);
    }
  }

  wgetjson_json_release();

  return ret;
}

cJSON* getbackStatus()
{
  return backStatus;
}
