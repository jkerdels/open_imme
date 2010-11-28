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


#include <string.h>
#include "imme.h"


void main(void) 
{
	uint8_t   row;
	uint8_t   col;
	uint8_t   idx = 0;

	imme_init();

	imme_red_led(1);
	ms_wait(1000);
	imme_red_led(0);

	while (1) {

		EA=0;
		idx = (idx + 5) % 100;
		EA=1;

		for (row = 10; row < 20; ++row){
			imme_set_pixel(idx + row,10,3);
			imme_set_pixel(idx + row+1,20,3);
		}
		for (col = 10; col < 20; ++col){
			imme_set_pixel(idx + 10,col+1,3);
			imme_set_pixel(idx + 20,col,3);
		}
		ms_wait(250);


		for (row = 10; row < 20; ++row){
			imme_set_pixel(idx + row,10,0);
			imme_set_pixel(idx + row+1,20,0);
		}
		for (col = 10; col < 20; ++col){
			imme_set_pixel(idx + 10,col+1,0);
			imme_set_pixel(idx + 20,col,0);
		}
		
	}
}

