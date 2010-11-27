#ifndef IMME_H
#define IMME_H

/*



*/

#include <stdint.h>
#include <cc1110.h>


#define DISP_WIDTH   132L
#define DISP_HEIGHT   65L
#define DISP_PAGES     9L

//extern __xdata volatile uint8_t dispBuf[];



// sampling frequency settings:
// SRATE_PRE is the prescaler of timer 3, e.g. SRATE_PRE = 2 => 6.5Mhz
// prescaler values:
// 0 = tickfreq /   1
// 1 = tickfreq /   2
// 2 = tickfreq /   4
// 3 = tickfreq /   8
// 4 = tickfreq /  16
// 5 = tickfreq /  32
// 6 = tickfreq /  64
// 7 = tickfreq / 128
// SRATE_MOD specifies the resulting sampling frequency, e.g
// SRATE_MOD = 208 => 6.5Mhz / 208 == 31.25kHz
// SRATE_MOD of [0..255]
// 3, 203 => ca. 16khz
// 4, 203 => ca.  8khz
#define SRATE_PRE    4
#define SRATE_MOD  203

// audio buffer size:
// this buffer is designed as a double buffer! So if you
// set the buffer to 625 for example, then 1250 bytes will 
// be used!
// 200 samples at 8khz = 25ms buffer
#define ABUF_SIZE 200L


// busy waiting wait, time in milliseconds
void ms_wait(uint16_t time); 


// call this once to initialize the imme
void imme_init(void);


// state == 1 -> on, 0 -> off
void imme_red_led(uint8_t state);
void imme_green_led(uint8_t state);


// signature of audio callback function
typedef void (*AudioCallback)(uint8_t *data);

// register a callback function which fills the
// audio buffer with ABUF_SIZE bytes
void imme_set_audio_callback(AudioCallback _ac);


// returns pressed char on keyboard
// if nothing is pressed 0 is returned
// special chars defined below
// ALT and CAPS are taken care of... the apropriate
// modified char will be returned
uint8_t getChar(void);

#define KEY_BYE     0xA0
#define KEY_POWER   0x90
#define KEY_MENU    0x88
#define KEY_SPEAKER 0x84
#define KEY_WHOISON 0x82
#define KEY_ALT     0x81
#define KEY_CAPS    0x80    
// up = '^'
#define KEY_UP      0x5E
// down = '/'
#define KEY_DOWN    0x2F
// left = '<'
#define KEY_LEFT    0x3C
// right = '>'
#define KEY_RIGHT   0x3E
// back = (backspace)
#define KEY_BACK    0x08
// enter = (carriage return)
#define KEY_ENTER   0x0D



// some gfx functions

// set pixel in "real" coordinates
// x in [0..DISP_WIDTH-1]
// y in [0..DISP_HEIGHT-1]
// color [0..3] -> 0 == white, 3 == black
void imme_set_pixel(uint8_t x, uint8_t y, uint8_t color);




// the interrupt service routines have to be included
// to main -> see sdcc documentation
void dma_isr(void) __interrupt (DMA_VECTOR) __using (2);
void timer3_isr(void) __interrupt (T3_VECTOR) __using (1);



#endif

