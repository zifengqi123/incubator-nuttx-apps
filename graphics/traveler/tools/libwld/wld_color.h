/****************************************************************************
 * apps/graphics/traveler/tools/libwld/wld_color.h
 *
 *   Copyright (C) 2016 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __APPS_GRAPHICS_TRAVELER_TOOLS_LIBWLD_WLD_COLOR_H
#define __APPS_GRAPHICS_TRAVELER_TOOLS_LIBWLD_WLD_COLOR_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "trv_types.h"
#include "trv_graphics.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* This defines the size of the RGB cube that can be supported by trv_pixel_t
 * Use of the RGB cube gives us a fine control of the color space.  However
 * the lighting logic in this program requires fine control over luminance.
 * If the RGB_CUBE_SIZE is small, then an alternative luminance model is
 * used.
 */

#define RGB_CUBE_SIZE 6
#define MIN_LUM_LEVELS 8

/* Maxium values of each RGB component */

/* These are definitions needed to support the luminance model */

#if RGB_CUBE_SIZE < MIN_LUM_LEVELS
#  define NUNIT_VECTORS  16
#  define NLUMINANCES    16    /* ((WLD_PIXEL_MAX+1)/NUNIT_VECTORS) */
#  define MAX_LUM_INDEX 255

  /* The following macros perform conversions between unit vector index and
   * luminance code into pixels and vice versa.  NOTE:  These macros assume
   * on the above values for NUNIT_VECTORS and NLUMINANCES
   */

#  define WLD_UVLUM2NDX(u,l) (((int)(u) << 4) + (l))
#  define WLD_NDX2UV(p)      ((p) >> 4)
#  define WLD_NDX2LUM(p)     ((p) & 0x0f)

  /* Each color is one of NCOLOR_FORMS */

#  define NCOLOR_FORMS 5
#endif

/* Device pixel color conversions */

#if defined(CONFIG_GRAPHICS_TRAVELER_RGB16_565)
/* This macro creates RGB16 (5:6:5)
 *
 *   R[7:3] -> RGB[15:11]
 *   G[7:2] -> RGB[10:5]
 *   B[7:3] -> RGB[4:0]
 */

#  define WLD_MKRGB(r,g,b) \
  ((((uint16_t)(r) << 8) & 0xf800) | (((uint16_t)(g) << 3) & 0x07e0) | (((uint16_t)(b) >> 3) & 0x001f))

/* And these macros perform the inverse transformation */

#  define RGB2RED(rgb)   (((rgb) >> 8) & 0xf8)
#  define RGB2GREEN(rgb) (((rgb) >> 3) & 0xfc)
#  define RGB2BLUE(rgb)  (((rgb) << 3) & 0xf8)

#  define RGB_MAX_RED    0xf8
#  define RGB_MAX_GREEN  0xfc
#  define RGB_MAX_BLUE   0xf8

#elif defined(CONFIG_GRAPHICS_TRAVELER_RGB32_888)
/* This macro creates RGB24 (8:8:8)
 *
 *   R[7:3] -> RGB[15:11]
 *   G[7:2] -> RGB[10:5]
 *   B[7:3] -> RGB[4:0]
 */

#  define WLD_MKRGB(r,g,b) \
  ((uint32_t)((r) & 0xff) << 16 | (uint32_t)((g) & 0xff) << 8 | (uint32_t)((b) & 0xff))

/* And these macros perform the inverse transformation */

#  define RGB2RED(rgb)   (((rgb) >> 16) & 0xff)
#  define RGB2GREEN(rgb) (((rgb) >> 8)  & 0xff)
#  define RGB2BLUE(rgb)  ( (rgb)        & 0xff)

#  define RGB_MAX_RED    0xff
#  define RGB_MAX_GREEN  0xff
#  define RGB_MAX_BLUE   0xff

#else
#  error No color format defined
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct color_rgb_s
{
  uint8_t red;    /* red   component of color 0-63 */
  uint8_t green;  /* green component of color 0-63 */
  uint8_t blue;   /* blue  component of color 0-63 */
};

typedef struct color_rgb_s color_rgb_t;

/* Used in color conversions */

struct color_lum_s
{
  float red;
  float green;
  float blue;
  float luminance;
};

typedef struct color_lum_s color_lum_t;

#if RGB_CUBE_SIZE < MIN_LUM_LEVELS
/* The following enumeration defines indices into the g_unit_vector array */

enum unit_vector_index_e
{
  GREY_NDX = 0,
  BLUE_NDX,
  GREENERBLUE_NDX,
  BLUEGREEN_NDX,
  BLUEVIOLET_NDX,
  VIOLET_NDX,
  LIGHTBLUE_NDX,
  GREEN_NDX,
  BLUERGREN_NDX,
  YELLOWGREEN_NDX,
  YELLOW_NDX,
  LIGHTGREEN_NDX,
  RED_NDX,
  REDVIOLET_NDX,
  ORANGE_NDX,
  PINK_NDX
};

struct color_form_s
{
  float max;
  float mid;
  float min;
};

typedef struct color_form_s color_form_t;

#endif

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* This is the palette table which is used to adjust the texture values
 * with distance
 */

extern dev_pixel_t *g_devpixel_lut;

#if RGB_CUBE_SIZE < MIN_LUM_LEVELS
color_lum_t *g_pixel2um_lut;

/* The following defines the "form" of each color in the g_unit_vector array */

const color_form_t g_wld_colorform[NCOLOR_FORMS];

/* This array defines each color supported in the luminance model */

const color_lum_t g_unit_vector[NUNIT_VECTORS];
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void wld_rgblookup_allocate(void);
void wld_rgblookup_endmapping(void);
void wld_rgblookup_free(void);

trv_pixel_t wld_rgb2pixel(color_rgb_t *pixel);
void wld_pixel2lum(trv_pixel_t pixel_value, color_lum_t *lum);
trv_pixel_t wld_lum2pixel(color_lum_t *lum);

#endif /* __APPS_GRAPHICS_TRAVELER_TOOLS_LIBWLD_WLD_COLOR_H */