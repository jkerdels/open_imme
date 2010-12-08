#include <stdio.h>
#include <string.h>

#include "rocketGfx.h"
#include "fahrplan_data.h"
#include "imme.h"


// state vars
__xdata uint8_t dayIdx[4];
__xdata uint8_t daySize[4];
__xdata uint8_t curDay;
__xdata uint8_t curEntry;
__xdata uint8_t lastEntry;
__xdata uint8_t nrOfEntries;
__xdata uint8_t curHr;



void fp_init(void)
{
	uint8_t EA_old;
	uint8_t i;
	EA_old = EA;
	EA = 0;

	memset(dayIdx,0,4);
	i = 0;
	while(fpData[i].dayloc != 0xFF) {
		if (((fpData[i].dayloc & 0x0F) == DAY2) && (dayIdx[1] == 0)) {
			dayIdx[1] = i;
		}
		if (((fpData[i].dayloc & 0x0F) == DAY3) && (dayIdx[2] == 0)) {
			dayIdx[2] = i;
		}
		if (((fpData[i].dayloc & 0x0F) == DAY4) && (dayIdx[3] == 0)) {
			dayIdx[3] = i;
		}
		++i;
	}
	nrOfEntries = i;
	daySize[3] = nrOfEntries - dayIdx[3];
	daySize[2] = dayIdx[3] - dayIdx[2];
	daySize[1] = dayIdx[2] - dayIdx[1];
	daySize[0] = dayIdx[1];

	curDay    = 0;
	curEntry  = 0;
	lastEntry = 1; // just different from curEntry
	curHr     = 0;

	EA = EA_old;
}


void fp_process_keys(void)
{
	uint8_t keyVal;
	uint8_t EA_old = EA;
	EA = 0;

	keyVal = imme_getChar();
	
	switch (keyVal) {
		case KEY_DOWN : {
			curEntry = (curEntry + 1) % nrOfEntries;
			curHr = fpData[curEntry].fromHr;
			curDay = fpData[curEntry].dayloc & 0xF;
		} break;
		case KEY_UP : {
			curEntry = (curEntry + nrOfEntries - 1) % nrOfEntries;
			curHr = fpData[curEntry].fromHr;
			curDay = fpData[curEntry].dayloc & 0xF;
		} break;
		case KEY_RIGHT : {
			uint8_t cidx;
			uint8_t dstSaal;
			dstSaal = (((fpData[curEntry].dayloc / 16) & 0xF) + 1) % 3;
			cidx = ((curEntry - dayIdx[curDay] + 1) % daySize[curDay]) + 
                   dayIdx[curDay];
			// search saal
			while (((fpData[cidx].dayloc / 16) & 0xF) != dstSaal)
				cidx = ((cidx - dayIdx[curDay] + 1) % daySize[curDay]) + 
                       dayIdx[curDay];
			// search time
			while (curHr > fpData[cidx].fromHr)
				cidx = ((cidx - dayIdx[curDay] + 1) % daySize[curDay]) + 
                       dayIdx[curDay];
			curEntry = cidx;
		} break;
		case KEY_LEFT : {
			uint8_t cidx;
			uint8_t dstSaal;
			dstSaal = (((fpData[curEntry].dayloc / 16) & 0xF) + 2) % 3;
			cidx = ((curEntry - dayIdx[curDay] + 1) % daySize[curDay]) + 
                   dayIdx[curDay];
			// search saal
			while (((fpData[cidx].dayloc / 16) & 0xF) != dstSaal)
				cidx = ((cidx - dayIdx[curDay] + 1) % daySize[curDay]) + 
                       dayIdx[curDay];
			// search time
			while (curHr > fpData[cidx].fromHr)
				cidx = ((cidx - dayIdx[curDay] + 1) % daySize[curDay]) + 
                       dayIdx[curDay];
			curEntry = cidx;
		} break;
		case '1' : {
			curEntry = dayIdx[0];
			curDay = 0;
		} break;
		case '2' : {
			curEntry = dayIdx[1];
			curDay = 1;
		} break;
		case '3' : {
			curEntry = dayIdx[2];
			curDay = 2;
		} break;
		case '4' : {
			curEntry = dayIdx[3];
			curDay = 3;
		} break;
		case 'p' : {
			curDay = (curDay + 1) & 3;
			curEntry = dayIdx[curDay];
		} break;
		case 'q' : {
			curDay = (curDay + 3) & 3;
			curEntry = dayIdx[curDay];
		} break;
		case KEY_POWER : {
			// trigger refresh
			lastEntry = curEntry + 1;
			ms_wait(500);
		} break;
	}
	EA = EA_old;
}

void fp_render(void)
{
	uint8_t EA_old = EA;
	EA = 0;
	// draw entry
	if (lastEntry != curEntry) {
		uint8_t strSize,titleLines,i;
		__code uint8_t * __xdata tmpStr;
		__code uint8_t * __xdata lines[6];

		imme_clr_scr(0);
		imme_set_font(FONT_TINY);
		imme_set_cursor(0,0);
		printf_tiny("DAY %u",(fpData[curEntry].dayloc & 0xF) + 1);
		imme_set_cursor(106,0);
		printf_tiny("SAAL %u",((fpData[curEntry].dayloc / 16) & 0xF) + 1);
		imme_set_font(FONT_BIG);
		imme_set_cursor(31,0);
		// printf_tiny doesn't support %02u formatting...
		if(fpData[curEntry].fromHr < 10)
			printf_tiny("0");
		printf_tiny("%u:",fpData[curEntry].fromHr);
		if(fpData[curEntry].fromMin < 10)
			printf_tiny("0");
		printf_tiny("%u-",fpData[curEntry].fromMin);
		if(fpData[curEntry].toHr < 10)
			printf_tiny("0");
		printf_tiny("%u:",fpData[curEntry].toHr);
		if(fpData[curEntry].toMin < 10)
			printf_tiny("0");
		printf_tiny("%u",fpData[curEntry].toMin);
		imme_set_font(FONT_TINY);

		// i know it is ugly... ah.. just don't ask ;-)
		lines[0] = fpData[curEntry].line1;
		lines[1] = fpData[curEntry].line2;
		lines[2] = fpData[curEntry].line3;
		lines[3] = fpData[curEntry].line4;
		lines[4] = fpData[curEntry].line5;
		lines[5] = fpData[curEntry].line6;

		titleLines = fpData[curEntry].hlNr;

		for (i = 0; i < 6; ++i) {
			if (titleLines > i) 
				imme_set_cursor((DISP_WIDTH-(strlen(lines[i]) << 2)) >> 1,
                                i+1);
			 else 
				imme_set_cursor(0,i+1);
			printf_tiny("%s",lines[i]);
		}

		imme_set_cursor(0,7);
		printf_tiny("%s",eventStr[(fpData[curEntry].flags / 2) & 0x7]);

		imme_set_cursor(36,7);	
		printf_tiny("|");

		tmpStr = langStr[fpData[curEntry].flags & 1];
		strSize = (36 - (strlen(tmpStr) * 4)) / 2;
		imme_set_cursor(44+strSize,7);
		printf_tiny("%s",tmpStr);

		imme_set_cursor(84,7);	
		printf_tiny("|");

		tmpStr = trackStr[(fpData[curEntry].flags / 16) & 0x7];
		strSize = 39 - (strlen(tmpStr) * 4);
		imme_set_cursor(92+strSize,7);
		printf_tiny("%s",tmpStr);

		imme_draw_hLine(8,2);
		imme_draw_hLine( 8 + (titleLines * 8),1);
		imme_draw_hLine( 9 + (titleLines * 8),2);
		imme_draw_hLine(10 + (titleLines * 8),1);
		imme_draw_hLine(57,2);


		lastEntry = curEntry;
	}		
	
	EA = EA_old;
}

void fp_intro(void)
{
	uint8_t i,t;

	imme_set_font(FONT_BIG);
	t = rocket[1];
	for (i = 1; i < t; ++i) {
		if (i == 10) {
			imme_set_cursor(5,1);
			printf_tiny("WE");
		} else if (i == 20) {
			imme_set_cursor(5,1);
			printf_tiny("WE COME");
		} else if (i == 30) {
			imme_set_cursor(19,3);
			printf_tiny("IN");
		} else if (i == 40) {
			imme_set_cursor(11,5);
			printf_tiny("PEACE");
		}
		imme_draw_gfx(i,DISP_HEIGHT-i,rocket,0);
		ms_wait(75);
		imme_clear_region(i,DISP_HEIGHT-i,1,rocket[1],0);
	}
	i = 0;
	t = 0;
	while(1) {
		uint8_t x = rocket[1]-1;
		uint8_t y = DISP_HEIGHT-rocket[1];
		uint8_t keyVal = imme_getChar();
		if (keyVal == ' ')
			break;
		switch (i) {
			case 0 : {
				x += 1;
			} break;
			case 1 : {
				x += 1;
				y += 1;
			} break;
			case 2 : {
				y += 1;
			} break;
		}
		imme_draw_gfx(x,y,rocket,0);
		imme_set_cursor(103,7);
		printf_tiny("27c3");
		ms_wait(150);
		imme_clear_region(x,y,1,rocket[1],0);
		imme_clear_region(x+rocket[0],y,1,rocket[1],0);
		imme_clear_region(x,y,rocket[0],1,0);
		i = (i + 1) & 3;
		if (i == 0) {
			t = (t + 1) & 1;
			imme_set_font(FONT_TINY);
			if (t == 0) {
				imme_set_cursor(11,7);
				printf_tiny("<SPACE>");
			} else {
				imme_set_cursor(11,7);
				printf_tiny("<PRESS>");
			}
			imme_set_font(FONT_BIG);
		}
	}
}

void fp_run(void)
{
	fp_intro();
	while (1) {
		fp_process_keys();
		fp_render();
		ms_wait(25);
	}
}


