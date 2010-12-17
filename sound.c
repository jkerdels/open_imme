#include "sound.h"
#include "tools.h"

#define DMA_BUFLEN  256
#define DMA_BUFMOD  255

static __xdata uint8_t dma_buf0[DMA_BUFLEN];
static __xdata uint8_t dma_buf0_idx;  // current position of buffer
static __xdata uint8_t dma_buf0_lidx; // last position of buffer

static uint8_t sampleCnt;

// this array provides the current writing index
// for dma_buf0_idx. A bad-hack-warning may be issued here.
__code const uint8_t SndIdx[DMA_BUFLEN] = {
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
	28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,
	53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,
	78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,
	102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
	121,122,123,124,125,126,127,
	128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,
	147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,
	166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,
	185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,
	204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,
	223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,
	242,243,244,245,246,247,248,249,250,251,252,253,254,255
};

void imme_sound_init(void)
{
    int i;

    // Use timer 4 for ~25.3kHz PWM output on P1.0
    //
    P1DIR  |= 0x01;
    T4CTL   = 0x30; // prescale, free-running
    T4CCTL0 = 0x24; // clear on compare, set on 0 (dma trigger for index)
    T4CCTL1 = 0x14; // toggle on compare (dma trigger)

    for (i=0; i<128; i++)
        dma_buf0[i] = 0;

    DMA_DESC[DMA_SOUND].SRCADDR  = BSWAP((uint16_t)dma_buf0);
    DMA_DESC[DMA_SOUND].DESTADDR = BSWAP((uint16_t)&X_T4CC0);
    DMA_DESC[DMA_SOUND].LENGTH   = BSWAP(DMA_BUFLEN);
    DMA_DESC[DMA_SOUND].FLAGS    = BSWAP(0x4A40);

	dma_buf0_idx  = 0;
	dma_buf0_lidx = 0;

	sampleCnt = 0;

    DMA_DESC[DMA_SOUND_IDX].SRCADDR  = BSWAP((uint16_t)SndIdx);
    DMA_DESC[DMA_SOUND_IDX].DESTADDR = BSWAP((uint16_t)&dma_buf0_idx);
    DMA_DESC[DMA_SOUND_IDX].LENGTH   = BSWAP(DMA_BUFLEN);
    DMA_DESC[DMA_SOUND_IDX].FLAGS    = BSWAP(0x4940);

    // Arm DMA channel
    //
    DMAARM |= (0x04 | 0x08); 
}

void imme_test_sound_trigger(void)
{
	uint8_t EA_old = EA;
	EA = 0;

	while (dma_buf0_lidx != dma_buf0_idx) {
		uint8_t saw = sampleCnt++;
		if (saw < 5) saw = 5;
		if (saw > 250) saw = 250;
		dma_buf0[dma_buf0_lidx] = saw;
		dma_buf0_lidx = (dma_buf0_lidx + 1) & DMA_BUFMOD;
	}

	EA = EA_old;
}


