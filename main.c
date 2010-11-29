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


#include <stdio.h>
#include <string.h>
#include "imme.h"


void main(void) 
{

	imme_init();

	imme_red_led(1);
	ms_wait(1000);
	imme_red_led(0);
	imme_green_led(0);

	ms_wait(250);		

	printf("|--------------------|");
	printf("| IM-Me Control 0.1a |");
	printf("|____________________|");

//	imme_set_pixel(0,8,3);

	while (1) {
		ms_wait(250);		
	}
}

