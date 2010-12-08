#ifndef TOOLS_H
#define TOOLS_H

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

#include <cc1110.h>
#include <stdint.h>

#define CAST(new_type,old_object) (*((new_type *)&old_object))

// busy waiting wait, time in milliseconds
void ms_wait(uint16_t time); 

void imme_stand_by(void);

// the interrupt service routines have to be included
// to main -> see sdcc documentation
void power_button_isr(void) __interrupt (P1INT_VECTOR);


#endif



