#include <string.h>
#include "imme.h"

#define CAST(new_type,old_object) (*((new_type *)&old_object))

// buffer used for the display with
// 2 bit per pixel (4 grayscales, yeah!)
__xdata static uint8_t dispBuf[DISP_WIDTH*DISP_PAGES*2L];

// double buffer used for audio output
__xdata static uint8_t audioBuf[ABUF_SIZE*2L];
uint16_t aBufSwitch;

// variable to hold the audio callback
static AudioCallback ac = 0;

// some low level disp funcs
void manual_SPI(uint8_t val)
{
	U0DBUF = val;
	while(!(U0CSR & 0x02)); // 1 == Last byte written to Data Buffer register 
                            // transmitted
	U0CSR &= ~0x02;         // clear status
}

void set_display_cmd(void)
{
	uint8_t tmp = 0;
	// set a0 low for sending cmd
	P0 &= ~0x04;
	// little waiting time
	++tmp;
	++tmp;
	++tmp;
}

void set_display_data(void)
{
	uint8_t tmp = 0;
	// set a0 high for sending data
	P0 |= 0x04;
	// little waiting time
	++tmp;
	++tmp;
	++tmp;
}

void set_display_start(uint8_t stVal)
{
	manual_SPI(0x40 | (stVal & 0x3F)); // display start
}

void set_display_page(uint8_t page)
{
	manual_SPI(0xb0 | (page & 0x0F)); // page address
}

void set_display_col(uint16_t col)
{
	manual_SPI(0x10 | (col >> 8) & 0x0F); // column high 8
	manual_SPI(0x00 | (col & 0x0F)); // column low 8
}


// some busy waiting
void ms_wait(uint16_t time)
{
	while (time--) {
		uint16_t cnt,k = 0;
		for (cnt = 0; cnt < 1200; ++cnt) {
			k++;
		}
	}
}


// dma cfgs
__xdata uint8_t dmaCfg0[8];
__xdata uint8_t dmaCfg1N[8];

// DMA ISR
// here we switch the audio double buffers and
// call the buffer-fill callback
void dma_isr(void) __interrupt (DMA_VECTOR) 
{
	static uint8_t curPage = 0;
	static uint8_t switchCnt = 0;
	static uint8_t frameCnt = 0;
	//static uint8_t frameCnt = 100;
	uint16_t tmpAddr;
	EA = 0;
	IRCON &= ~0x01; // clear CPU irq flag
	if (DMAIRQ & 0x01) {
		// clear interrupt flag
		DMAIRQ &= ~1;
		// backup adress for callback
		tmpAddr = (dmaCfg0[0] << 8) | dmaCfg0[1];
		// switch audio buffers
		dmaCfg0[0] ^= aBufSwitch >> 8;
		dmaCfg0[1] ^= aBufSwitch;
		// do callback
		//P2 &= ~0x18;
		// rearm the dma
		DMAARM |= 0x01;
		if (ac)
			ac(CAST(uint8_t*,tmpAddr));
	}

	if (DMAIRQ & 0x02) {
		uint16_t cnt, k = 0;
		// clear interrupt flag
		DMAIRQ     &= ~0x02;
		set_display_cmd();
		// update src address and page
		curPage = (curPage + 1) % DISP_PAGES;
		tmpAddr = (dmaCfg1N[0] << 8) | dmaCfg1N[1];
		if (curPage == 0) {
			switchCnt = (switchCnt + 1) % 3;
			set_display_start(0);
			tmpAddr = (uint16_t)(dispBuf);
			if (switchCnt)
				tmpAddr += DISP_WIDTH;
		}
		tmpAddr += DISP_WIDTH*2;
		dmaCfg1N[0]  = tmpAddr >> 8;
		dmaCfg1N[1]  = tmpAddr;
		manual_SPI(0xa4);
		set_display_page(curPage);
		set_display_col(0);
		set_display_col(0);
		set_display_data();
		// rearm the dma
		DMAARM |= 0x02;
		for (cnt = 0; cnt < 110; ++cnt) // we need a finely tuned delay
			++k;                        // to prevent flickering...
		// trigger first manual if mem is not used
		DMAREQ |= 0x02;
	}
	EA = 1;

}

// timer for audio frequency and keyboard stuff
__xdata volatile uint8_t keyStatus[8];
__xdata volatile uint8_t keyChange[8];

void timer3_isr(void) __interrupt (T3_VECTOR) __using (1)
{
	static uint8_t callCnt = 0;
	EA = 0;
	IRCON &= ~0x08; // clear CPU irq flag
	if (TIMIF & 0x01) {
		TIMIF &= ~0x01; // clear flag
		DMAREQ |= 0x01; // trigger DMA 0
		/*
		if ((callCnt & 15) == 0)
			DMAREQ = 0x02; // trigger DMA 1
		*/

		// do some keyboard stuff ... sometimes
		// @8khz this would be roughly 30hz
		if (callCnt++ == 0) {
			uint8_t tmpSet, gndSet;
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
		}
	}
	EA = 1;
}

const uint8_t keyLU[192] = {
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

uint8_t getChar(void)
{
	uint8_t alt  = (keyChange[5] & ~keyStatus[5]) & 0x20;
	uint8_t caps = (keyChange[4] & ~keyStatus[4]) & 0x08;
	uint8_t row;

	for (row = 0; row < 8; ++row) {
		uint8_t check = keyStatus[row] & keyChange[row];
		if (check) {
			uint8_t luIdx;
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
			return keyLU[luIdx];
		}		
	}
	return 0;
}


void display_clear(uint8_t pageVal)
{
	uint8_t row;
	uint16_t col;
	set_display_start(0);
	for (row = 0; row < DISP_PAGES; ++row) {
		set_display_cmd();
		set_display_page(row);
		set_display_col(0);
		set_display_data();
		for (col = 0; col < DISP_WIDTH; ++col)
			manual_SPI(pageVal);
	}
}

void reset_display(void)
{
	// wait for DMA to take a break
	//while (dmaEnabled);
	// short reset pulse to display
	P1 &= ~0x02;
	ms_wait(1);
	P1 |= 0x02;
	ms_wait(1);

	// display init sequence
	set_display_cmd();

	// this sequence is from ST7565S doc, values estimated from Dave's work
	manual_SPI(0xA2); // LCD Bias    ... value already set by reset
	manual_SPI(0xA1); // ADC select
	manual_SPI(0xC0); // normal common output mode
	manual_SPI(0x24); // set 5V resistor ratio to 4.0 (Table 11 on page 34)
	manual_SPI(0x81); // electronic volume mode set
	manual_SPI(0x22); // electronic volume register set to middle value 
	manual_SPI(0x2f); // power control set (booster, voltage regulator and
                      // voltage follower on)

	// additionally turn display on in normal mode
	manual_SPI(0xaF);
    manual_SPI(0xa4);

	manual_SPI(0xa6); // normal mode

/*
	// this is the sequence from the spectrum analyzer of michael ossmann
    // original from dave's blog
	manual_SPI(0xe2); // RESET cmd
	manual_SPI(0x24); // set internal resistor ratio
	manual_SPI(0x81); // set Vol Control
	manual_SPI(0x60); // set Vol Control - ctd
	manual_SPI(0xe6); // ?? -- don't know what this command is
	manual_SPI(0x00); // ?? -- don't know what this command is
	manual_SPI(0x2f); // set internal PSU operating mode
	manual_SPI(0xa1); // LCD bias set
	manual_SPI(0xaf); // Display ON
	manual_SPI(0xa4); // Normal (not all pixels) mode
*/

}

void imme_set_audio_callback(AudioCallback _ac)
{
	ac = _ac;
}


void testPattern(void)
{
	uint8_t col;
	uint8_t *curPixel;
	curPixel = dispBuf + DISP_WIDTH*8;

	for (col = 0; col < 132; ++col) 
		*(curPixel++) = 0xFF;
	for (col = 0; col < 132; ++col) 
		*(curPixel++) = 0x00;
	for (col = 0; col < 132; ++col) 
		*(curPixel++) = 0x00;
	for (col = 0; col < 132; ++col) 
		*(curPixel++) = 0xFF;
	for (col = 0; col < 132; ++col) 
		*(curPixel++) = 0xFF;
	for (col = 0; col < 132; ++col) 
		*(curPixel++) = 0xFF;

}

void imme_init(void)
{
	EA = 0;
	//-------------------------------------------------------------------------
	// init the clock
	
	// Both (XOSC & HS RCOSC) oscillators powered up
	SLEEP &= ~0x04;
	// XOSC Oscillator is powered up and stable
	while (!(SLEEP & 0x40));
	// set XOSC Oscillator with 26Mhz Tick and Clock Speed
	CLKCON &= 0x80;
	// wait for XOSC being set
	while (CLKCON & 0x40);
	// turn of HS RCOSC
	SLEEP |= 0x04;
	
	//-------------------------------------------------------------------------
	// init interrupts
	// enable DMA interrupt
    // enable timer 3 interrupt
	IEN1 |= 0x09;
//	IEN2 |= 0x04;

	
	
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

	// init keyStatus
	memset(keyStatus,0x00,8);
	memset(keyChange,0x00,8);
	
	// configure pins accordingly
	P0SEL  = 0x28; 
	P1SEL  = 0x01;
	
	// set input or output where this is fixed, e.g. for the LEDs
	P0DIR |=  0x14;
	P1DIR |=  0x03;
	P2DIR |=  0x18;

	P0DIR &= 0x3D; // set keyboard input for P0

	//-------------------------------------------------------------------------
	// configure audio

	// timer4 for pwm generation:
	// prescaler = 1
    // started, no overflow interrupt, 
	// free running
	T4CTL = 0x10;
	// compare channel 0 enabled
	// set on 0x00, clear on compare
	T4CCTL0 = 0x24;
	// init compare value = 0
	T4CC0 = 128;


	// timer3 for sampling frequency generation
	// prescaler = SRATE_PRE
	// started, no overflow interrupt,
    // modulo with SRATE_MOD
	T3CTL = ((SRATE_PRE << 5) & 0xE0) | 0x1A;
	T3CC0 = SRATE_MOD;

	// set up audio buffers
	memset(audioBuf,0,ABUF_SIZE*2);
	aBufSwitch = (uint16_t)audioBuf ^ ((uint16_t)(audioBuf + ABUF_SIZE));

	// set up DMA transfer
	// the Timer 3 overflow interrupt is used to trigger the copy manually
    // the compare trigger can't be used, as it is required to also output
    // the compare... disabling output, disables the trigger also --> suckalot
    // one byte of the current audioBuf to T4CC0 
	// the DMA cfg structure looks like this:
	// byte 0: high Byte of src addr
	// byte 1: low Byte of src addr
    // byte 2: high Byte of dst addr
    // byte 3: low Byte of dst addr
    // byte 4: transfer count mode (VLEN) | LEN [12:8]
    // byte 5: LEN [7..0]
	// byte 6: WORDSIZE | transfer mode | trigger
    // byte 7: src inc mode | dst inc mode | irq mask | M8 | priority
	dmaCfg0[0] = (uint16_t)(audioBuf) >> 8;
	dmaCfg0[1] = (uint16_t)(audioBuf);
	dmaCfg0[2] = 0xDF; //(uint16_t)(&T4CC0) >> 8;
	dmaCfg0[3] = 0xED; //(uint16_t)(&T4CC0)
	dmaCfg0[4] = (ABUF_SIZE >> 8) & 0x1F;
	dmaCfg0[5] = ABUF_SIZE & 0xFF;
	dmaCfg0[6] = 0x00; // it sucks, but we can't use timer 3 dma trigger
	dmaCfg0[7] = 0x49;
	// set the cfg
	DMA0CFGH = (uint16_t)(dmaCfg0) >> 8;
	DMA0CFGL = (uint16_t)(dmaCfg0);
	// clear irq for dma0;
	DMAIRQ &= ~0x01;
	// arm DMA
	DMAARM |= 0x01;


	//-------------------------------------------------------------------------
	// init SPI for display
	U0CSR  = 0x00; // set for SPI master
	U0BAUD = 0x3B; // baud mantissa 
	U0GCR  = 0x30; // MSB first + 0x10 as baud exponent
//	U0GCR  = 0x2F;

	// enable chip select 
	P0 &= ~0x10;

	// reset display
	reset_display();
	display_clear(0x00);

	memset(dispBuf,0x00,DISP_WIDTH*DISP_PAGES*2);

	// prepare DMA transfer for display data
	dmaCfg1N[0] = (uint16_t)(dispBuf) >> 8;
	dmaCfg1N[1] = (uint16_t)(dispBuf);
	dmaCfg1N[2] = 0xDF; //(uint16_t)(&U0DBUF) >> 8;
	dmaCfg1N[3] = 0xC1; //(uint16_t)(&U0DBUF)
	dmaCfg1N[4] = (DISP_WIDTH >> 8) & 0x1F;
	dmaCfg1N[5] = DISP_WIDTH & 0xFF;
	dmaCfg1N[6] = 0x0F; // trigger with Usart0 TX complete
	dmaCfg1N[7] = 0x48; // low priority

	// set the cfg
	DMA1CFGH = (uint16_t)(dmaCfg1N) >> 8;
	DMA1CFGL = (uint16_t)(dmaCfg1N);
	// clear irq for dma1;
	DMAIRQ &= ~0x02;

	set_display_cmd();
	set_display_page(0);
	set_display_col(0);
	set_display_data();

	// arm DMA
	DMAARM |= 0x02;

	// trigger first manual
	DMAREQ |= 0x02;
	
	testPattern();

	// enable interrupts globally
	EA = 1;

}

void imme_red_led(uint8_t state)
{
	if (state)
		P2 &= ~0x08;
	else
		P2 |=  0x08;
}

void imme_green_led(uint8_t state)
{
	if (state)
		P2 &= ~0x10;
	else
		P2 |=  0x10;
}

void imme_set_pixel(uint8_t x, uint8_t y, uint8_t color)
{
	uint8_t *page;
	uint8_t m;
	EA = 0;
	page = dispBuf + (y >> 3) * (DISP_WIDTH*2L) + x;
	m = 1 << (y & 7);
	if (color & 1)
		*page |= m;
	else
		*page &= ~m;
	page += DISP_WIDTH;
	if (color & 2)
		*page |= m;
	else
		*page &= ~m;
	EA = 1;
}









