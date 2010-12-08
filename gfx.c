#include <string.h>
#include "tools.h"
#include "gfx.h"

#include "imme_font.h"
#include "tinyfont.h"

// buffer used for the display with
// 2 bit per pixel (4 grayscales)
__xdata static uint8_t dispBuf[DISP_WIDTH*DISP_PAGES*2L];

__xdata static uint8_t dmaCfg1N[8];

static uint8_t fontWidth;
static uint8_t *font;


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


/*
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
*/


void reset_display(void)
{
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

}




// DMA ISR
// here we switch the audio double buffers and
// call the buffer-fill callback
void dma_isr(void) __interrupt (DMA_VECTOR)
{
	static uint8_t curPage = 0;
	static uint8_t switchCnt = 0;
	static uint8_t frameCnt = 0;
	uint16_t tmpAddr;
	uint8_t EA_old = EA;
	EA = 0;
	IRCON &= ~0x01; // clear CPU irq flag
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
		} else {
			tmpAddr += DISP_WIDTH*2L;
		}
		dmaCfg1N[0]  = tmpAddr >> 8;
		dmaCfg1N[1]  = tmpAddr;
		manual_SPI(0xa4);
		set_display_page(curPage);
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
	} else {
		DMAIRQ = 0; // clear everything if unkown irq -> should not happen
	}
	EA = EA_old;
}




void imme_gfx_init(void)
{
	uint8_t oldEA = EA;
	EA = 0;
	fontWidth = tinyfontWidth;
	font      = tinyfont;

	//-------------------------------------------------------------------------
	// init SPI for display
	U0CSR  = 0x00; // set for SPI master
	U0BAUD = 0x3B; // baud mantissa 
	U0GCR  = 0x30; // MSB first + 0x10 as baud exponent

	// enable chip select 
	P0 &= ~0x10;

	// reset display
	reset_display();
//	display_clear(0x00);

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

	// enable DMA IRQ
	IEN1 |= 0x01;

	// set the cfg
	DMA1CFGH = (uint16_t)(dmaCfg1N) >> 8;
	DMA1CFGL = (uint16_t)(dmaCfg1N);
	// clear irq for dma1;
	DMAIRQ &= ~0x02;

	set_display_cmd();
	set_display_start(0);
	set_display_page(0);
	set_display_col(0);
	set_display_data();

	// arm DMA
	DMAARM |= 0x02;

	// trigger first manual
	DMAREQ |= 0x02;


	EA = oldEA;
}




void imme_clr_scr(uint8_t val)
{
	uint8_t EA_old = EA;
	EA = 0;
	memset(dispBuf,val,DISP_WIDTH*DISP_PAGES*2L);
	EA = EA_old;
}

void imme_set_pixel(uint8_t x, uint8_t y, uint8_t color)
{
	__xdata uint8_t *page;
	uint8_t m;
	uint8_t EA_old = EA;
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
	EA = EA_old;
}

void imme_draw_hLine(uint8_t y, uint8_t color)
{
	uint8_t i;
	for (i = 0; i < DISP_WIDTH; ++i)
		imme_set_pixel(i,y,color);
}


__xdata static uint8_t cursorPage = 0;
__xdata static uint8_t cursorXPos = 0;

void imme_set_cursor(uint8_t x, uint8_t page)
{
	uint8_t EA_old = EA;
	EA = 0;
	cursorPage = page;
	cursorXPos = x;
	EA = EA_old;
}

void imme_set_font(uint8_t fIdx)
{

	uint8_t EA_old = EA;
	EA = 0;
	switch (fIdx) {
		case FONT_TINY :
			fontWidth = tinyfontWidth;
			font      = tinyfont;
			break;
		case FONT_BIG : 
			fontWidth = defaultFontWidth;
			font      = defaultFont;
			break;
		default : 
			fontWidth = tinyfontWidth;
			font      = tinyfont;
		 	break;
	}
	EA = EA_old;

}

// sdcc provides printf if we provide this 
void putchar(char c) 
{
	__xdata uint8_t *dispPos;
	uint8_t x,changed;
	uint8_t EA_old = EA;
	EA = 0;
	dispPos = dispBuf + cursorPage * (DISP_WIDTH*2L) + cursorXPos;
	if (c == 0x20){ // space
		cursorXPos += fontWidth + 1;
	} else if (c == 0x0D) { // CR
		cursorXPos = 0;
	} else if (c == 0x0A) { // NL
		cursorXPos = 0;
		++cursorPage;
	} else if ((c > 0x20) && (c < 0x7F)) {
		uint16_t fidx;
		changed = 0;
		if (cursorXPos >= DISP_WIDTH-fontWidth) {
			cursorXPos = 0;
			++cursorPage;
			changed = 1;
		}
		if (cursorPage > 7) {
			cursorPage = 0;
			changed = 1;
		}
		if (changed)
			dispPos = dispBuf + cursorPage * (DISP_WIDTH*2L) + cursorXPos;
		// print lsb
		fidx = (c - 0x21) * (fontWidth*2);
		for (x = 0; x < fontWidth; ++x)
			dispPos[x] = font[fidx+x];
		// print msb
		for (x = 0; x < fontWidth; ++x)
			dispPos[x+DISP_WIDTH] = font[fidx+x+fontWidth];
		// increase cursor
		cursorXPos += fontWidth + 1;
	}
	EA = EA_old;
}



void imme_clear_region(uint8_t x, 
                       uint8_t y, 
                       uint8_t w, 
                       uint8_t h,
					   uint8_t val)
{
	__xdata uint8_t *dstLSB;
	__xdata uint8_t *dstMSB;
	uint8_t subPages;
	uint8_t p,lx,m;
	uint16_t dlDist = DISP_WIDTH * 2L;
	uint8_t EA_old = EA;
	EA = 0;

	p = DISP_WIDTH - x;
	if (w > p)
		w = p;

	dlDist -= w;

	p = DISP_HEIGHT - y;
	if (h > p)
		h = p;

	p = y & 7;

	m = (1 << p) - 1;

	lx = 8 - p;
	if (lx > h) {
		m |= ~((1 << (p+h)) - 1);
		h = 0;
	} else {
		h -= lx;
	}

	subPages = h >> 3;

	// first line
	dstLSB = dispBuf + ((DISP_WIDTH * 2L) * (y >> 3)) + x;
	dstMSB  = dstLSB  + DISP_WIDTH;
	for (lx = 0; lx < w; ++lx) {
		*dstLSB = (*dstLSB & m) | (val & ~m);
		++dstLSB;
		*dstMSB = (*dstMSB & m) | (val & ~m);
		++dstMSB;
	}
	dstLSB += dlDist;
	dstMSB += dlDist;
	for (p = 0; p < subPages; ++p) {
		for (lx = 0; lx < w; ++lx) {
			*(dstLSB++) = val;
			*(dstMSB++) = val;
		}
		dstLSB += dlDist;
		dstMSB += dlDist;
	}
	p = h & 7;	
	if (p > 0) {
		m = (1 << p) - 1;
		for (lx = 0; lx < w; ++lx) {
			*dstLSB = (*dstLSB & ~m) | (val & m);
			++dstLSB;
			*dstMSB = (*dstMSB & ~m) | (val & m);
			++dstMSB;
		}
	}
	

	EA = EA_old;
}


void imme_draw_gfx(uint8_t x, uint8_t y, __code uint8_t *gfx, uint8_t useMask)
{
	__code uint8_t *gfxMask;
	__code uint8_t *gfxLSB;
	__code uint8_t *gfxMSB;
	__xdata uint8_t *dstLSB;
	__xdata uint8_t *dstMSB;
	uint8_t subWidth, subHeight, subPages;
	uint8_t p,lx,shiftL,shiftR;
	uint16_t lDist = gfx[0] + gfx[0] + gfx[0];
	uint16_t dlDist = DISP_WIDTH * 2L;
	uint8_t EA_old = EA;
	EA = 0;

	subWidth = gfx[0];
	p = DISP_WIDTH - x;
	if (subWidth > p)
		subWidth = p;

	dlDist -= subWidth;
	lDist  -= subWidth;

	subHeight = gfx[1];
	p = DISP_HEIGHT - y;
	if (subHeight > p)
		subHeight = p;



	shiftL = y & 7;
	shiftR = 8 - shiftL;
	
	p = 8 - shiftL;
	if (p > subHeight)
		subHeight = 0;
	else
		subHeight -= p;
	
	subPages = subHeight >> 3;

	if ((subHeight & 7) > 0)
		++subPages;

	// first line
	gfxMask = gfx+2;
	gfxLSB  = gfxMask + gfx[0];
	gfxMSB  = gfxLSB  + gfx[0];
	dstLSB  = dispBuf + ((DISP_WIDTH * 2L) * (y >> 3)) + x;
	dstMSB  = dstLSB  + DISP_WIDTH;
	for (lx = 0; lx < subWidth; ++lx) {
		uint8_t m;
		if (useMask)
			m = ~(*(gfxMask++) << shiftL);
		else
			m = (1 << shiftL) - 1;
		*dstLSB = (*dstLSB & m) | (*(gfxLSB++) << shiftL);
		*dstMSB = (*dstMSB & m) | (*(gfxMSB++) << shiftL);
		++dstLSB;
		++dstMSB;
	}
	gfxMask -= subWidth;
	gfxLSB  -= subWidth;
	gfxMSB  -= subWidth;
	dstLSB  += dlDist;
	dstMSB  += dlDist;
	for (p = 0; p < subPages; ++p) {
		for (lx = 0; lx < subWidth; ++lx) {
			uint8_t m;
			if (useMask)
				m = ~(*(gfxMask++) >> shiftR);
			else
				m = ~((1 << shiftL) - 1);
			*dstLSB = (*dstLSB & m) | (*(gfxLSB++) >> shiftR);
			*dstMSB = (*dstMSB & m) | (*(gfxMSB++) >> shiftR);
			++dstLSB;
			++dstMSB;
		}
		gfxMask += lDist;
		gfxLSB  += lDist;
		gfxMSB  += lDist;
		dstLSB  -= subWidth;
		dstMSB  -= subWidth;
		for (lx = 0; lx < subWidth; ++lx) {
			uint8_t m;
			if (useMask)
				m = ~(*(gfxMask++) << shiftL);
			else
				m = (1 << shiftL) - 1;
			*dstLSB = (*dstLSB & m) | (*(gfxLSB++) << shiftL);
			*dstMSB = (*dstMSB & m) | (*(gfxMSB++) << shiftL);
			++dstLSB;
			++dstMSB;
		}
		gfxMask -= subWidth;
		gfxLSB  -= subWidth;
		gfxMSB  -= subWidth;
		dstLSB  += dlDist;
		dstMSB  += dlDist;
	}

	EA = EA_old;

}

void imme_display_standby(void)
{
	set_display_cmd();
	manual_SPI(0xAC); // static indicator off
	manual_SPI(0xAE); // display off
	manual_SPI(0xA5); // display all points ON
	set_display_data();
}




