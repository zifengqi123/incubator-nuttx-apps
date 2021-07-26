

include $(APPDIR)/Make.defs

# Hello, World! built-in application info

PROGNAME  = $(CONFIG_EXAMPLES_LEDCONTROL_PROGNAME)
PRIORITY  = $(CONFIG_EXAMPLES_LEDCONTROL_PRIORITY)
STACKSIZE = $(CONFIG_EXAMPLES_LEDCONTROL_STACKSIZE)
MODULE    = $(CONFIG_EXAMPLES_LEDCONTROL)

CSRCS += led_ctrl_curl.c
CSRCS += led_ctrl_pthread.c
CSRCS += mdnssd_min.c
CSRCS += led_ctrl_elf.c

MAINSRC = led_ctrl_main.c

include $(APPDIR)/Application.mk