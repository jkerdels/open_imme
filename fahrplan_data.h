#ifndef FAHRPLAN_DATA_H
#define FAHRPLAN_DATA_H

#include <stdint.h>

// bit 0
#define LANG_ENG        0
#define LANG_GER        1

// bits 1-3
#define EV_LECTURE      0
#define EV_PODIUM       2
#define EV_CONTEST      4
#define EV_LTALK        6
#define EV_WORKSHOP     8
#define EV_OTHER       10

// bits 4-6
#define TR_COMMUNITY    0
#define TR_CULTURE     16
#define TR_HACKING     32
#define TR_MAKING      48
#define TR_SCIENCE     64
#define TR_SOCIETY     80

extern __code const char langStr[2][8];

extern __code const char eventStr[6][9];

extern __code const char trackStr[6][10];

// bits 0-3
#define DAY1    0
#define DAY2    1
#define DAY3    2
#define DAY4    3

// bits 4-7
#define SAAL1  0
#define SAAL2  16
#define SAAL3  32


typedef struct _FPItem {
	uint8_t dayloc;
	uint8_t fromHr;
	uint8_t fromMin;
	uint8_t toHr;
	uint8_t toMin;
	//char text[198];  // 6 Lines with 33 chars each, use \n to sep. title, etc.
					   // the sdcc printf seems to have problems with long text
	char line1[34];
	char line2[34];
	char line3[34];
	char line4[34];
	char line5[34];
	char line6[34];
	uint8_t hlNr;    // how many lines for the talk title
	uint8_t flags;   // one LANG, one EV, one TR 
} FPItem;

extern __code const FPItem fpData[];


#endif


