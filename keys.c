#include <string.h>
#include "tools.h"
#include "gfx.h"
#include "keys.h"

__xdata static uint8_t keyStatus[8];
__xdata static uint8_t keyChange[8];


void imme_keys_init(void)
{

	//-------------------------------------------------------------------------
	// init I/O
	
	// port 0 overview:
	// bit 0 = decoupled voltage divider input --> ADC A0
	// bit 1 = used for keyboard
	// bit 2 = A0  -- Low for a command byte, high for a data byte to ST7565S
	// bit 3 = SI  -- Serial Data (MOSI in SPI terminology) to ST7565S
	// bit 4 = !CS -- hold low while transferring data (SSN in SPI terminology) 
	//                to ST7565S
	// bit 5 = SCL -- SPI clock (SCK in SPI terminology) to ST7565S
	// bit 6 = used for keyboard
	// bit 7 = used for keyboard
	
	// port 1 overview:
	// bit 0 = output to piezo
	// bit 1 = !reset to ST7565S
	// bit 2 = used for keyboard
	// bit 3 = used for keyboard
	// bit 4 = used for keyboard
	// bit 5 = used for keyboard
	// bit 6 = used for keyboard
	// bit 7 = used for keyboard
	
	// port 2 overview:
	// bit 0 = digital input/output to backlight circuit?
	// bit 1 = debug interface DD
	// bit 2 = debug interface DC
	// bit 3 = digital output to status LED (red?)
	// bit 4 = digital output to status LED (green?)

	
	// configure pins accordingly
	P0SEL  = 0x28; 
	P1SEL  = 0x01;
	
	// set input or output where this is fixed, e.g. for the LEDs
	P0DIR |=  0x14;
	P1DIR |=  0x03;
	P2DIR |=  0x18;

	P0DIR &= 0x3D; // set keyboard input for P0

	memset(keyStatus,0x00,8);
	memset(keyChange,0x00,8);

}

void imme_key_io(void)
{
	uint8_t tmpSet, gndSet;
	uint8_t EA_old = EA;
	EA = 0;
	// check GND row
	P1DIR &= 0x03;
	P0 |= 0x02;
	tmpSet = (~P1 & 0xFC);
	keyChange[0] |= keyStatus[0] ^ tmpSet;
	keyStatus[0]  = tmpSet;
	gndSet = ~(tmpSet >> 2); // we need this to distinguish between GND row
				  		     // and the other rows
	// check 0_1 row
	P0DIR |= 0x02;
	P0 &= ~0x02;
	// setting 0_6/0_7 input is done in init()
	tmpSet = (((~P1 & 0xFC) >> 2) | (~P0 & 0xC0)) & gndSet;
	keyChange[1] |= keyStatus[1] ^ tmpSet;
	keyStatus[1]  = tmpSet;
	P0DIR &= ~0x02;
	// check 1_2 row
	P1DIR |= 0x04;
	P1 &= ~0x04;
	tmpSet = (((~P1 & 0xF8) >> 2) | (~P0 & 0xC0)) & gndSet;
	keyChange[2] |= keyStatus[2] ^ tmpSet;
	keyStatus[2]  = tmpSet;
	P1DIR &= ~0x04;
	// check 1_3 row
	P1DIR |= 0x08;
	P1 &= ~0x08;
	tmpSet = (((~P1 & 0xF0) >> 2) | (~P0 & 0xC0)) & gndSet;
	keyChange[3] |= keyStatus[3] ^ tmpSet;
	keyStatus[3]  = tmpSet;
	P1DIR &= ~0x08;
	// check 1_4 row
	P1DIR |= 0x10;
	P1 &= ~0x10;
	tmpSet = (((~P1 & 0xE0) >> 2) | (~P0 & 0xC0)) & gndSet;
	keyChange[4] |= keyStatus[4] ^ tmpSet;
	keyStatus[4]  = tmpSet;
	P1DIR &= ~0x10;
	// check 1_5 row
	P1DIR |= 0x20;
	P1 &= ~0x20;
	tmpSet = (((~P1 & 0xC0) >> 2) | (~P0 & 0xC0)) & gndSet;
	keyChange[5] |= keyStatus[5] ^ tmpSet;
	keyStatus[5]  = tmpSet;
	P1DIR &= ~0x20;
	// check 1_6 row
	P1DIR |= 0x40;
	P1 &= ~0x40;
	tmpSet = (((~P1 & 0x80) >> 2) | (~P0 & 0xC0)) & gndSet;
	keyChange[6] |= keyStatus[6] ^ tmpSet;
	keyStatus[6]  = tmpSet;
	P1DIR &= ~0x40;
	// check 1_7 row
	P1DIR |= 0x80;
	P1 &= ~0x80;
	tmpSet = (~P0 & 0xC0);
	keyChange[7] |= keyStatus[7] ^ tmpSet;
	keyStatus[7]  = tmpSet;
	P1DIR &= ~0x80;
	EA = EA_old;
}



__code const uint8_t keyLU[192] = {
	// normal key section
	0x00, 0x00,   KEY_POWER,  'o',       'p',     'm',         'n',      'k', 
	'y',   'g',    KEY_MENU,  'c', KEY_RIGHT,     ',',    KEY_LEFT,      ' ', 
	0x00,  'q',         'u',  'w',       'i',     't',         'r',      'e',
	0x00, 0x00,         'h',  'a',       'j',     'f',         'd',      's',
	0x00, 0x00,         'v', 0x00,       'b',     'x',         'z', KEY_CAPS,
	0x00, 0x00, KEY_WHOISON, 0x00,  KEY_BACK, KEY_ALT, KEY_SPEAKER,     0x00,
	0x00, 0x00,    KEY_DOWN, 0x00,    KEY_UP, KEY_BYE,        0x00,     0x00,
	0x00, 0x00,   KEY_ENTER, 0x00,       'l',    0x00,        0x00,     0x00,
	// alt key section
	0x00, 0x00, 0x00,  '+',  '=',  '.',  '!',  '"', 
	'$',   '_', 0x00,  '9', 0x00,  '?', 0x00,  ' ', 
	0x00,  '1',  '&',  '2',  '-',  '#',  '@',  '3',
	0x00, 0x00,  ':',  '4',  ';', '\\',  '6',  '5',
	0x00, 0x00,  '(', 0x00,  ')',  '8',  '7', 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  '0', 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, '\'', 0x00, 0x00, 0x00,
	// caps key section
	0x00, 0x00, 0x00,  'O',  'P',  'M',  'N',  'K', 
	 'Y',  'G', 0x00,  'C', 0x00, 0x00, 0x00,  ' ', 
	0x00,  'Q',  'U',  'W',  'I',  'T',  'R',  'E',
	0x00, 0x00,  'H',  'A',  'J',  'F',  'D',  'S',
	0x00, 0x00,  'V', 0x00,  'B',  'X',  'Z', 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  'L', 0x00, 0x00, 0x00
};

uint8_t imme_getChar(void)
{
	uint8_t alt  = (keyChange[5] & ~keyStatus[5]) & 0x20;
	uint8_t caps = (keyChange[4] & ~keyStatus[4]) & 0x08;
	uint8_t row;
	uint8_t EA_old = EA;
	EA = 0;
	for (row = 0; row < 8; ++row) {
		uint8_t check = keyStatus[row] & keyChange[row];
		if (check) {
			uint8_t luIdx;
			uint8_t pressedKey;
			// isolate one key
			check &= -check;
			// skip alt or caps key
			if (((row != 5) || (check != 0x20)) &&
				((row != 4) || (check != 0x08))) {
				keyChange[row] &= ~check;	// reset change info
				keyChange[5] &= ~0x20;
				keyChange[4] &= ~0x08;
			} else {
				continue;
			}
			// get lookup index with de bruijn sequence 0x1D
			// resulting in mapping:
			// 0x01 = 0 <-> bit 0
			// 0x02 = 1 <-> bit 1
			// 0x04 = 3 <-> bit 2
			// 0x08 = 7 <-> bit 3
			// 0x10 = 6 <-> bit 4
			// 0x20 = 5 <-> bit 5
			// 0x40 = 2 <-> bit 6
			// 0x80 = 4 <-> bit 7
			luIdx = (row << 3) +(((0x1D * check) >> 5) & 0x07);
			
			if (alt) {
				luIdx +=  64;
			} else if (caps) {
				luIdx += 128;
			}
			
			// get key
			pressedKey = keyLU[luIdx];
			// check for power button
			EA = EA_old;
			if (pressedKey == KEY_POWER) {
				imme_clr_scr(0);
				ms_wait(500);
				imme_stand_by();
				ms_wait(500);
			}

			return pressedKey;
		}		
	}
	EA = EA_old;
	return 0;
}



