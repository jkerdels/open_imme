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

void imme_gfx_init(void);

// clears the screen to val
void imme_clr_scr(uint8_t val);

// set pixel in "real" coordinates
// x in [0..DISP_WIDTH-1]
// y in [0..DISP_HEIGHT-1]
// color [0..3] -> 0 == white, 3 == black
void imme_set_pixel(uint8_t x, uint8_t y, uint8_t color);

// sets cursor for printf
void imme_set_cursor(uint8_t x, uint8_t page);

void imme_draw_hLine(uint8_t y, uint8_t color);

void imme_draw_gfx(uint8_t x, uint8_t y,__code uint8_t *gfx, uint8_t useMask);

void imme_clear_region(uint8_t x, 
                       uint8_t y, 
                       uint8_t w, 
                       uint8_t h,
					   uint8_t val);

#define FONT_TINY 0
#define FONT_BIG  1

// sets the font to use 
void imme_set_font(uint8_t fIdx);

// if this is provided, sdcc provides printf
void putchar(char c);

void imme_display_standby(void);


// the interrupt service routines have to be included
// to main -> see sdcc documentation
void dma_isr(void) __interrupt (DMA_VECTOR);


#endif

