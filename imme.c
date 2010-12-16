#include "imme.h"



// just setting up the clock and calling the init functions of the sub-parts
// i.e. keys, gfx, ...
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
	imme_sound_init();
	
	// enable interrupts globally
	EA = 1;
}


/*
 * switching the LEDs, the LEDs
 * are connected "inverted":
 *        ___  _____
 * VCC ---|R|--|LED|-- IO_Pin
 *       
 * So a '0' at the IO-Pin allows
 * a current to flow and will enable 
 * the LED...
 */ 
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




