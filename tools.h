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

/*
 * As the DMA config structures from DMA1 upwards have to be in one place
 * it is defined here.
 * If you need a DMA structure, increase the NR_OF_DMA_ENTRIES and
 * add a corresponding Offset define.
 */
#define NR_OF_DMA_ENTRIES    1
#define DISP_DMA_OFFSET      0
extern __xdata uint8_t dmaCfg1N[NR_OF_DMA_ENTRIES*8];


/*
 * Useful cast makro to cast everything into anything
 * Do not use this extensivly ;-)
 */
#define CAST(new_type,old_object) (*((new_type *)&old_object))


/*
 * simple busy waiting 
 */
void ms_wait(uint16_t time); 


/*
 * Puts the imme into standby mode.
 * A press of the power button wakes the imme up again.
 */
void imme_stand_by(void);


/*
 * the interrupt service routines have to be included
 * to main -> see sdcc documentation
 */
void power_button_isr(void) __interrupt (P1INT_VECTOR);


#endif



