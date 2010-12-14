#include "imme.h"

__xdata struct dma_desc DMA_DESC[DMA_CHANNELS];

/**
 * Simple busy waiting
 */
void ms_wait(uint16_t time)
{
	while (time--) {
		uint16_t cnt, k = 0;
		for (cnt = 0; cnt < 1200; ++cnt) {
			k++;
		}
	}
}


/**
 * Put the imme into standby mode.
 *
 * A press of the power button wakes the imme up again.
 * Original src from Michael Ossmanns Spectrum analyser, modified a little bit
 */
void imme_stand_by(void)
{
	volatile uint8_t desc_high = DMA0CFGH;
	volatile uint8_t desc_low = DMA0CFGL;
	__xdata uint8_t dma_buf[7] = {0x07,0x07,0x07,0x07,0x07,0x07,0x04};
	__xdata uint8_t DMA_DESC[8] = {0x00,0x00,0xDF,0xBE,0x00,0x07,0x20,0x42};
	uint8_t EA_old = EA;
	EA = 0;

	// kill the display, see gfx.h/gfx.c for details
	imme_display_standby();

	/* switch to HS RCOSC */
	SLEEP &= ~0x04;
	while (!(SLEEP & 0x20));
	CLKCON = (CLKCON & ~0x07) | 0x40 | 0x04;
	while (!(CLKCON & 0x40));
	SLEEP |= 0x04;

	// kill all other irqs
	IEN0 &= ~0x3F;
	IEN1 &= ~0x3F;
	IEN2 &= ~0x3F;

	// set irq for power button
	P1IFG  = 0x00; // clear irq flags
	IRCON2 &= ~0x08;
	P1IEN |= 0x40; // pin 1_6 (power button)
	IEN2  |= 0x10;
	PICTL |= 0x02; // falling edges

	EA = 1;

	/* store descriptors and abort any transfers */
	desc_high = DMA0CFGH;
	desc_low = DMA0CFGL;
	DMAARM |= (0x80 | 0x01);

	/* DMA prep */
	DMA_DESC[0] = (uint16_t)&dma_buf >> 8;
	DMA_DESC[1] = (uint16_t)&dma_buf;
	DMA0CFGH = (uint16_t)&DMA_DESC >> 8;
	DMA0CFGL = (uint16_t)&DMA_DESC;
	DMAARM = 0x01;

	/*
	 * Any interrupts not intended to wake from sleep should be
	 * disabled by this point.
	 */

	/* disable flash cache */
	MEMCTR |= 0x02;

	/* select sleep mode PM3 and power down XOSC */
	SLEEP |= (0x03 | 0x04);

	__asm
   	nop
   	nop
   	nop
	__endasm;

	if (SLEEP & 0x03) {
		__asm
		mov 0xD7,#0x01 /* DMAREQ */
		nop
		orl 0x87,#0x01 /* last instruction before sleep */
		nop            /* first instruction after wake */
		__endasm;
	}

	/* enable flash cache */
	MEMCTR &= ~0x02;

	/* restore DMA */
	DMA0CFGH = desc_high;
	DMA0CFGL = desc_low;
	DMAARM = 0x01;

	/* make sure HS RCOSC is stable */
	while (!(SLEEP & 0x20));

	imme_init();

	EA = EA_old;
}

/*
 * Interrupt service routine of Power-Button used to wake imme up
 */
void power_button_isr(void) __interrupt (P1INT_VECTOR)
{
	// clear module flag first (port interrupt)
	P1IFG &= ~0x40;
	// clear cpu_flag
	IRCON2 &= ~0x08;
	// clear sleep
	SLEEP &= ~SLEEP;
}
