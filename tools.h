/**
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
#ifndef TOOLS_H
#define TOOLS_H

#include <cc1110.h>
#include <stdint.h>

/**
 * Byte swap macro
 *
 */
#define BSWAP(x)  (((x)<<8) | ((x)>>8))

/**
 * DMA channels
 *
 */
#define DMA_USER       0
#define DMA_LCD        1
#define DMA_SOUND      2
#define DMA_SOUND_IDX  3
#define DMA_CHANNELS   4

/**
 * DMA descriptors
 *
 * \note These fields are in big-endian order and
 *       should be accessed using the BSWAP macro.
 */
extern __xdata struct dma_desc {
    volatile uint16_t   SRCADDR;
    volatile uint16_t   DESTADDR;
    volatile uint16_t   LENGTH;
    volatile uint16_t   FLAGS;
} DMA_DESC[DMA_CHANNELS];


/**
 * Simple busy waiting
 */
void ms_wait(uint16_t time); 


/**
 * Put the IM-ME into standby mode.
 * A press of the power button wakes the IM-ME up again.
 */
void imme_stand_by(void);


/**
 * Tthe interrupt service routines have to be included
 * in main -> see sdcc documentation
 */
void power_button_isr(void) __interrupt (P1INT_VECTOR);


#endif
