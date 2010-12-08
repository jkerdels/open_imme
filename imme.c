#include "imme.h"


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
	
	imme_keys_init();

	imme_gfx_init();

	
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




