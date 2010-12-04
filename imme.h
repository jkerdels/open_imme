#ifndef IMME_H
#define IMME_H

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

// sampling frequency settings:
// SRATE_PRE is the prescaler of timer 3, e.g. SRATE_PRE = 2 => 6.5Mhz
// prescaler values:
// 0 = tickfreq /   1
// 1 = tickfreq /   2
// 2 = tickfreq /   4
// 3 = tickfreq /   8
// 4 = tickfreq /  16
// 5 = tickfreq /  32
// 6 = tickfreq /  64
// 7 = tickfreq / 128
// SRATE_MOD specifies the resulting sampling frequency, e.g
// SRATE_MOD = 208 => 6.5Mhz / 208 == 31.25kHz
// SRATE_MOD of [0..255]
// 3, 203 => ca. 16khz
// 4, 203 => ca.  8khz
#define SRATE_PRE    4
#define SRATE_MOD  203

// audio buffer size:
// this buffer is designed as a double buffer! So if you
// set the buffer to 625 for example, then 1250 bytes will 
// be used!
// 200 samples at 8khz = 25ms buffer
#define ABUF_SIZE 200L


// busy waiting wait, time in milliseconds
void ms_wait(uint16_t time); 


// call this once to initialize the imme
void imme_init(void);


// state == 1 -> on, 0 -> off
void imme_red_led(uint8_t state);
void imme_green_led(uint8_t state);


// signature of audio callback function
typedef void (*AudioCallback)(uint8_t *data);

// register a callback function which fills the
// audio buffer with ABUF_SIZE bytes
void imme_set_audio_callback(AudioCallback _ac);


// returns pressed char on keyboard
// if nothing is pressed 0 is returned
// special chars defined below
// ALT and CAPS are taken care of... the apropriate
// modified char will be returned
uint8_t imme_getChar(void);

#define KEY_BYE     0xA0
#define KEY_POWER   0x90
#define KEY_MENU    0x88
#define KEY_SPEAKER 0x84
#define KEY_WHOISON 0x82
#define KEY_ALT     0x81
#define KEY_CAPS    0x80    
// up = '^'
#define KEY_UP      0x5E
// down = '/'
#define KEY_DOWN    0x2F
// left = '<'
#define KEY_LEFT    0x3C
// right = '>'
#define KEY_RIGHT   0x3E
// back = (backspace)
#define KEY_BACK    0x08
// enter = (carriage return)
#define KEY_ENTER   0x0A



// some gfx functions

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

#define FONT_TINY 0
#define FONT_BIG  1

// sets the font to use 
void imme_set_font(uint8_t fIdx);

// if this is provided, sdcc provides printf
void putchar(char c);

// the interrupt service routines have to be included
// to main -> see sdcc documentation
void dma_isr(void) __interrupt (DMA_VECTOR);
void timer3_isr(void) __interrupt (T3_VECTOR);
void power_button_isr(void) __interrupt (P1INT_VECTOR);


#endif

