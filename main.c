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
#include "fahrplan_data.h"
#include "rocketGfx.h"

void main(void) 
{
	__xdata uint8_t dayIdx[4];
	uint8_t curDay;
	uint8_t idx,keyVal,lastidx,curHr,phit;
	uint8_t nrOfEntries;
	uint8_t EA_old;

	// set up vars
	EA_old = EA;
	EA = 0;
	memset(dayIdx,0,4);
	curDay = 0;
	idx = 0;
	curHr = fpData[0].fromHr;
	//k = 0;
	keyVal = 0;
	phit = 0;

	while(fpData[idx].dayloc != 0xFF) {
		if (((fpData[idx].dayloc & 0x0F) == DAY2) && (dayIdx[1] == 0))
			dayIdx[1] = idx;
		if (((fpData[idx].dayloc & 0x0F) == DAY3) && (dayIdx[2] == 0))
			dayIdx[2] = idx;
		if (((fpData[idx].dayloc & 0x0F) == DAY4) && (dayIdx[3] == 0))
			dayIdx[3] = idx;
		++idx;
	}
	lastidx = 1;
	nrOfEntries = idx;
	idx = 0;
	EA = EA_old;

	imme_init();
	imme_red_led(0);
	imme_green_led(0);


	while (1) {
	
		keyVal = imme_getChar();
		
		EA_old = EA;
		EA = 0;
		switch (keyVal) {
			case KEY_DOWN : {
				idx = (idx + 1) % nrOfEntries;
				curHr = fpData[idx].fromHr;
			} break;
			case KEY_UP : {
				idx = (idx + nrOfEntries - 1) % nrOfEntries;
				curHr = fpData[idx].fromHr;
			} break;
			case KEY_RIGHT : {
				uint8_t cidx;
				uint8_t dstSaal;
				dstSaal = (((fpData[idx].dayloc / 16) & 0xF) + 1) % 3;
				cidx = (idx + 1) % nrOfEntries;
				// search saal
				while (((fpData[cidx].dayloc / 16) & 0xF) != dstSaal)
					cidx = (cidx + 1) % nrOfEntries;
				// search time
				while (curHr > fpData[cidx].fromHr)
					cidx = (cidx + 1) % nrOfEntries;
				idx = cidx;
			} break;
			case KEY_LEFT : {
				uint8_t cidx;
				uint8_t dstSaal;
				dstSaal = (((fpData[idx].dayloc / 16) & 0xF) + 2) % 3;
				cidx = (idx + 1) % nrOfEntries;
				// search saal
				while (((fpData[cidx].dayloc / 16) & 0xF) != dstSaal)
					cidx = (cidx + 1) % nrOfEntries;
				// search time
				while (curHr > fpData[cidx].fromHr)
					cidx = (cidx + 1) % nrOfEntries;
				idx = cidx;
			} break;
			case '1' : {
				idx = dayIdx[0];
				curDay = 0;
			} break;
			case '2' : {
				idx = dayIdx[1];
				curDay = 1;
			} break;
			case '3' : {
				idx = dayIdx[2];
				curDay = 2;
			} break;
			case '4' : {
				idx = dayIdx[3];
				curDay = 3;
			} break;
			case 'p' : {
				curDay = (curDay + 1) & 3;
				idx = dayIdx[curDay];
			} break;
			case 'q' : {
				curDay = (curDay + 3) & 3;
				idx = dayIdx[curDay];
			} break;
			case KEY_POWER : {
				// trigger refresh
				lastidx = idx + 1;
				ms_wait(500);
			} break;
		}
		EA = EA_old;
		

		EA_old = EA;
		EA = 0;
		// draw entry
		if (lastidx != idx) {
			uint8_t strSize,titleLines,i;
			uint8_t *tmpStr;
			uint8_t *lines[6];

			imme_clr_scr(0);
			imme_set_font(FONT_TINY);
			imme_set_cursor(0,0);
			printf("DAY %u",(fpData[idx].dayloc & 0xF) + 1);
			imme_set_cursor(106,0);
			printf("SAAL %u",((fpData[idx].dayloc / 16) & 0xF) + 1);
			imme_set_font(FONT_BIG);
			imme_set_cursor(31,0);
			printf("%02u:%02u-%02u:%02u",
		           fpData[idx].fromHr,
				   fpData[idx].fromMin,
				   fpData[idx].toHr,
				   fpData[idx].toMin);
			imme_set_font(FONT_TINY);

			// i know it is ugly... ah.. just don't ask ;-)
			lines[0] = fpData[idx].line1;
			lines[1] = fpData[idx].line2;
			lines[2] = fpData[idx].line3;
			lines[3] = fpData[idx].line4;
			lines[4] = fpData[idx].line5;
			lines[5] = fpData[idx].line6;

			titleLines = fpData[idx].hlNr;

			for (i = 0; i < 6; ++i) {
				if (titleLines > i) 
					imme_set_cursor((DISP_WIDTH-(strlen(lines[i]) << 2)) >> 1,
                                    i+1);
				 else 
					imme_set_cursor(0,i+1);
				printf("%s",lines[i]);
			}

			imme_set_cursor(0,7);
			printf("%s",eventStr[(fpData[idx].flags / 2) & 0x7]);

			imme_set_cursor(36,7);	
			printf("|");

			tmpStr = langStr[fpData[idx].flags & 1];
			strSize = (36 - (strlen(tmpStr) * 4)) / 2;
			imme_set_cursor(44+strSize,7);
			printf("%s",tmpStr);

			imme_set_cursor(84,7);	
			printf("|");

			tmpStr = trackStr[(fpData[idx].flags / 16) & 0x7];
			strSize = 39 - (strlen(tmpStr) * 4);
			imme_set_cursor(92+strSize,7);
			printf("%s",tmpStr);

			imme_draw_hLine(8,2);
			imme_draw_hLine( 8 + (titleLines * 8),1);
			imme_draw_hLine( 9 + (titleLines * 8),2);
			imme_draw_hLine(10 + (titleLines * 8),1);
			imme_draw_hLine(57,2);


			lastidx = idx;
		}		
		
		EA = EA_old;

		ms_wait(25);
	}
}

