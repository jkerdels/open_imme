#include <string.h>
#include "tools.h"
#include "gfx.h"

// used fonts
#include "font_tiny.h"
#include "font_big.h"

/*
 * This is the buffer used to hold the contents of the screen. It is organized
 * in pages (chunks) of 8 lines packet into the bytes, i.e. the first DISP_WIDTH
 * bytes in memory correspond to the first 8 lines of the display. The LSB Bit
 * of the bytes is line 0, the MSB Bit of the bytes is line 7.
 * To enable "fake" grayscale, the buffer holds 2 Bits for every Pixel, where
 * one bit is schon 1 times and the other bit is schon 2 times in fast 
 * alteration. This results in 4 grayscales:
 *   Both bits 0               == white, 
 *   first bit 1, second bit 0 == light gray, 
 *   first bit 0, second bit 1 == dark gray, 
 *   both bits 1               == black.
 * 
 * In the memory here the pages for first and second bits of the pixels are
 * alternating, i.e. the memory starts with DISP_WIDTH bytes representing the
 * first 8 rows of the display containing the "first bits" of the pixels, then
 * DISP_WIDTH bytes follow which represent also the first 8 rows of the display,
 * but contain the "second bits" of the pixels.. and so on with the next page.
 * 
 * The memory is continously transferred to the display via DMA.
 *
 * If you just use the high_level functions in gfx.h you do not have to worry
 * about this ;-)
 */
__xdata static uint8_t dispBuf[DISP_WIDTH*DISP_PAGES*2L];

/*
 * Memory of DMA cfg structure, see tools.h for details
 */
__xdata uint8_t *dispDmaCfg = dmaCfg1N + DISP_DMA_OFFSET;


/*
 * "currentFont" holds the pointer to the currently selected
 * font. Update the imme_set_font() function below, to add custom fonts
 * The "currentFont" pointer is initialized in the imme_gfx_init().
 */
static __code uint8_t *currentFont;

void imme_set_font(uint8_t fIdx)
{

	uint8_t EA_old = EA;
	EA = 0;
	switch (fIdx) {
		case FONT_TINY :
			currentFont = SmallFont;
			break;
		case FONT_BIG : 
			currentFont = BigFont;
			break;
		default : 
			currentFont = SmallFont;
		 	break;
	}
	EA = EA_old;
}


/*
 * some low level SPI functions used internally here in gfx.c
 */
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




/*
 * After transmission of every Page (8 lines) of display data to the display
 * this interrupt is called to update the DMA structure and reset the display
 * to receive the next line. This method also handles the switching between
 * "first" and "second" bits of the pixels to create the illusion of grayscale.
 * 
 * The DMA-Trigger used is the "byte transmitted" trigger of the SPI. To start
 * the transmission we have to trigger the first byte manually (see comment 
 * below)
 *
 */
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
		tmpAddr = (dispDmaCfg[0] << 8) | dispDmaCfg[1];
		if (curPage == 0) {
			switchCnt = (switchCnt + 1) % 3;
			set_display_start(0);
			tmpAddr = (uint16_t)(dispBuf);
			if (switchCnt)
				tmpAddr += DISP_WIDTH;
		} else {
			tmpAddr += DISP_WIDTH*2L;
		}
		dispDmaCfg[0]  = tmpAddr >> 8;
		dispDmaCfg[1]  = tmpAddr;
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
		// trigger first manual
		DMAREQ |= 0x02;
	} else {
		DMAIRQ = 0; // clear everything if unkown irq -> should not happen
	}
	EA = EA_old;
}



/*
 * This function initialized the gfx stuff and is called from imme_init(), so
 * you do not have to call this explicitly
 */
void imme_gfx_init(void)
{
	uint8_t oldEA = EA;
	EA = 0;

	// init of font
	currentFont = BigFont;

	//-------------------------------------------------------------------------
	// init SPI for display
	U0CSR  = 0x00; // set for SPI master
	U0BAUD = 0x3B; // baud mantissa 
	U0GCR  = 0x30; // MSB first + 0x10 as baud exponent

	// enable chip select 
	P0 &= ~0x10;

	// reset display
	reset_display();

	memset(dispBuf,0x00,DISP_WIDTH*DISP_PAGES*2);

	// prepare DMA transfer for display data
	dispDmaCfg[0] = (uint16_t)(dispBuf) >> 8;
	dispDmaCfg[1] = (uint16_t)(dispBuf);
	dispDmaCfg[2] = 0xDF; //(uint16_t)(&U0DBUF) >> 8;
	dispDmaCfg[3] = 0xC1; //(uint16_t)(&U0DBUF)
	dispDmaCfg[4] = (DISP_WIDTH >> 8) & 0x1F;
	dispDmaCfg[5] = DISP_WIDTH & 0xFF;
	dispDmaCfg[6] = 0x0F; // trigger with Usart0 TX complete
	dispDmaCfg[7] = 0x48; // low priority

	// enable DMA IRQ
	IEN1 |= 0x01;

	// set the cfg
	DMA1CFGH = (uint16_t)(dispDmaCfg) >> 8;
	DMA1CFGL = (uint16_t)(dispDmaCfg);
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


/*
 * Clear the screen buffer with value 'val'.
 * Essentially you will use most of the time
 * val == 0x00 for white or
 * val == 0xFF for black
 * everything else greates nice line patterns ;-)
 */
void imme_clr_scr(uint8_t val)
{
	uint8_t EA_old = EA;
	EA = 0;
	memset(dispBuf,val,DISP_WIDTH*DISP_PAGES*2L);
	EA = EA_old;
}


/*
 * Set a pixel to 'color' ([0..3]) with "normal" coordinates x,y
 * Like every high-level set_pixel method this thing is horribly
 * slow...
 */
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


/*
 * Draws a horizontal line at row 'y' with 'color' [0..3]
 * As it uses set_pixel, it is not fast either
 */
void imme_draw_hLine(uint8_t y, uint8_t color)
{
	uint8_t i;
	for (i = 0; i < DISP_WIDTH; ++i)
		imme_set_pixel(i,y,color);
}


/*
 * These two vars hold the current cursor position
 */
__xdata static uint8_t cursorPage = 0;
__xdata static uint8_t cursorXPos = 0;


/*
 * This function sets the cursor for printf.
 * 'x' selects the column to start, 
 * 'page' selects the page to start, i.e. page [0..7]
 */
void imme_set_cursor(uint8_t x, uint8_t page)
{
	uint8_t EA_old = EA;
	EA = 0;
	cursorPage = page;
	cursorXPos = x;
	EA = EA_old;
}


/*
 * If this function is provided, sdcc provides printf,
 * it is recommended to use printf_tiny() ;-)
 */
void putchar(char c) 
{
	__xdata uint8_t *dispPos;
	uint8_t x,changed;
	uint8_t EA_old = EA;
	uint8_t fontWidth = currentFont[0];
	__code uint8_t* font = currentFont+1;
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



/*
 * This function clears a region of the screen with value val similar to the
 * imme_clr_scr() function.
 * Clipping to right and bottom is taken care of.
 */
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


/*
 * This function draws a bitmapped graphic (created with the gfxconvert tool)
 * using the top,left position of (x,y). The parameter 'useMask' is a boolean
 * (0 == false, 1 == true) which disables/enables using of the mask of the gfx.
 * Clipping to right and bottom is taken care of.
 */
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


/*
 * Set the display to standby-mode, used by imme_stand_by(), see tools.h
 */
void imme_display_standby(void)
{
	set_display_cmd();
	manual_SPI(0xAC); // static indicator off
	manual_SPI(0xAE); // display off
	manual_SPI(0xA5); // display all points ON
	set_display_data();
}




