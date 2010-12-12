#ifndef GFX_H
#define GFX_H

/*
 * Copyright 2010 Jochen Kerdels
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#include <stdint.h>
#include <cc1110.h>


#define DISP_WIDTH   132L
#define DISP_HEIGHT   65L
#define DISP_PAGES     9L

/*
 * This function is called by imme_init(), so it does not need to be called
 * explicitly.
 */
void imme_gfx_init(void);


/*
 * This function clears the screen to 'val'. Val is used to fill a page
 * of the screen buffer, i.e. val == 1 will not fill the screen black, but 
 * instead will fill the screen with a line every 8 rows. To fill the screen
 * completly black, val has to be all ones (0xFF);
 */
void imme_clr_scr(uint8_t val);


/*
 * set pixel in "real" coordinates
 * x in [0..DISP_WIDTH-1]
 * y in [0..DISP_HEIGHT-1]
 * color [0..3] -> 0 == white, 3 == black
 */
void imme_set_pixel(uint8_t x, uint8_t y, uint8_t color);


/*
 * This function draws a horizontal line at row 'y' with
 * color [0..3] -> 0 == white, 3 == black
 */
void imme_draw_hLine(uint8_t y, uint8_t color);


/*
 * This function draws a bitmapped graphic (created with the gfxconvert tool)
 * using the top,left position of (x,y). The parameter 'useMask' is a boolean
 * (0 == false, 1 == true) which disables/enables using of the mask of the gfx.
 * Clipping to right and bottom is taken care of.
 */
void imme_draw_gfx(uint8_t x, uint8_t y,__code uint8_t *gfx, uint8_t useMask);


/*
 * This function clears a region of the screen with value val similar to the
 * imme_clr_scr() function.
 * Clipping to right and bottom is taken care of.
 */
void imme_clear_region(uint8_t x, 
                       uint8_t y, 
                       uint8_t w, 
                       uint8_t h,
					   uint8_t val);


#define FONT_TINY 0
#define FONT_BIG  1
/*
 * Set the font to use next (see defines above).
 * To add a font, update the imme_set_font() function
 * in  gfx.c and create a corresponding define above
 */
void imme_set_font(uint8_t fIdx);


/*
 * This function sets the cursor for printf.
 * 'x' selects the column to start, 
 * 'page' selects the page to start, i.e. page [0..7]
 */
void imme_set_cursor(uint8_t x, uint8_t page);


/*
 * If this function is provided, sdcc provides printf,
 * it is recommended to use printf_tiny() ;-)
 */
void putchar(char c);


/*
 * Set the display to standby-mode, used by imme_stand_by(), see tools.h
 */
void imme_display_standby(void);


/*
 * the interrupt service routines have to be included
 * to main -> see sdcc documentation
 */
void dma_isr(void) __interrupt (DMA_VECTOR);


#endif

