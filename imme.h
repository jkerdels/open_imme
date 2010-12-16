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

#include "tools.h"
#include "gfx.h"
#include "keys.h"
#include "sound.h"

// call this once to initialize the imme
void imme_init(void);

// state == 1 -> on, 0 -> off
void imme_red_led(uint8_t state);
void imme_green_led(uint8_t state);

// see tools.h, gfx.h, keys.h for more
// lib functions


#endif

