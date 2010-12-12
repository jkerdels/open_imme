#ifndef KEYS_H
#define KEYS_H

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

/*
 * This function is called by imm_init(), 
 * so it does not need to be called explicitly!
 */
void imme_keys_init(void);



/*
 * Returns pressed char on keyboard as ASCII.
 * If nothing is pressed, 0 is returned.
 * Special chars are defined below.
 * ALT and CAPS are taken care of -> the apropriate
 * modified char will be returned
 * Power-Button logic to send the device into standby is
 * incorporated in this function. If you do not want
 * standby caused by the Power-Button, undef the following
 * define.
 */
#define POWER_BUTTON_STANDBY

uint8_t imme_get_char(void);

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



/*
 * This function has to be called regularly to scan
 * the IOs for new keyboard input, e.g. in your applications
 * main loop or by a timer.
 */
void imme_key_io(void);






#endif


