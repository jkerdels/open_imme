#include "sound.h"
#include "imme.h"

#define DMA_BUFLEN  128L
#define DMA_BUFMOD  127

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
/*
	128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,
	147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,
	166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,
	185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,
	204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,
	223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,
	242,243,244,245,246,247,248,249,250,251,252,253,254,255
*/
};

/*
__code const uint8_t sin_lu[256] = {
	0x7F, 0x82, 0x85, 0x88, 0x8B, 0x8F, 0x92, 0x95, 0x98, 0x9B, 0x9E, 0xA1, 
	0xA4, 0xA7, 0xAA, 0xAD, 0xB0, 0xB3, 0xB6, 0xB8, 0xBB, 0xBE, 0xC1, 0xC3, 
	0xC6, 0xC8, 0xCB, 0xCD, 0xD0, 0xD2, 0xD5, 0xD7, 0xD9, 0xDB, 0xDD, 0xE0, 
	0xE2, 0xE4, 0xE5, 0xE7, 0xE9, 0xEB, 0xEC, 0xEE, 0xEF, 0xF1, 0xF2, 0xF4, 
	0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFB, 0xFC, 0xFD, 0xFD, 0xFE, 
	0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFD, 0xFD, 
	0xFC, 0xFB, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF2, 0xF1, 
	0xEF, 0xEE, 0xEC, 0xEB, 0xE9, 0xE7, 0xE5, 0xE4, 0xE2, 0xE0, 0xDD, 0xDB, 
	0xD9, 0xD7, 0xD5, 0xD2, 0xD0, 0xCD, 0xCB, 0xC8, 0xC6, 0xC3, 0xC1, 0xBE, 
	0xBB, 0xB8, 0xB6, 0xB3, 0xB0, 0xAD, 0xAA, 0xA7, 0xA4, 0xA1, 0x9E, 0x9B, 
	0x98, 0x95, 0x92, 0x8F, 0x8B, 0x88, 0x85, 0x82, 0x7F, 0x7C, 0x79, 0x76, 
	0x73, 0x6F, 0x6C, 0x69, 0x66, 0x63, 0x60, 0x5D, 0x5A, 0x57, 0x54, 0x51, 
	0x4E, 0x4B, 0x48, 0x46, 0x43, 0x40, 0x3D, 0x3B, 0x38, 0x36, 0x33, 0x31, 
	0x2E, 0x2C, 0x29, 0x27, 0x25, 0x23, 0x21, 0x1E, 0x1C, 0x1A, 0x19, 0x17, 
	0x15, 0x13, 0x12, 0x10, 0x0F, 0x0D, 0x0C, 0x0A, 0x09, 0x08, 0x07, 0x06, 
	0x05, 0x04, 0x03, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x03, 0x03, 0x04, 
	0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0C, 0x0D, 0x0F, 0x10, 0x12, 0x13, 
	0x15, 0x17, 0x19, 0x1A, 0x1C, 0x1E, 0x21, 0x23, 0x25, 0x27, 0x29, 0x2C, 
	0x2E, 0x31, 0x33, 0x36, 0x38, 0x3B, 0x3D, 0x40, 0x43, 0x46, 0x48, 0x4B, 
	0x4E, 0x51, 0x54, 0x57, 0x5A, 0x5D, 0x60, 0x63, 0x66, 0x69, 0x6C, 0x6F, 
	0x73, 0x76, 0x79, 0x7C, 
};
*/

// state of voice 1
__idata static uint16_t v1_freq = 0;
__idata static uint16_t v1_acc;
__idata static uint16_t rand = 0xFFFF;
/*
__idata static union {
	uint16_t acc;
	uint8_t  acc_bytes[2];
} v1_acc = {0};
*/
__bit static v1_noise  = 0;
__bit static v1_square = 0;
__bit static v1_tri    = 0;
__bit static v1_saw    = 0;
__idata static uint8_t v1_square_pw  = 127;
__idata static uint8_t v1_noise_mask =   0;
__idata static uint8_t v1_noise_val  =   0;
__idata static uint8_t v1_last_saw   =   0;
__idata static uint8_t v1_out        =   0;


void update_voice(void)
{
	uint8_t saw;
	//v1_acc.acc += v1_freq;
	//saw = v1_acc.acc_bytes[1];
	v1_acc += v1_freq;
	saw = v1_acc >> 8;
	if ((saw ^ v1_last_saw) & 0x80) {
		rand = (rand << 1) | (((rand >> 14) ^ (rand >> 13)) & 1);
		v1_noise_val = rand;
	}
	v1_last_saw = saw;
	v1_out = 0xFF;
	if (v1_noise) {
		v1_out &= v1_noise_val;
	}
	if (v1_square)
		v1_out &= (saw > v1_square_pw) ? 0x00 : 0xFF;
	if (v1_tri) {
		uint8_t tri = saw << 1;
        if (saw & 0x80)
	        tri ^= 0xff;
		v1_out &= tri;
	}
	if (v1_saw)
		v1_out &= saw;
}

// envelope of voice 1
__xdata static uint8_t v1_env_volume = 0;

__idata static uint8_t v1_env1_cnt    = 0;
__idata static uint8_t v1_env1_period = 1;
__idata static uint8_t v1_env2_cnt    = 0;
__idata static uint8_t v1_env2_period = 1;

#define ENV_ATTACK  0x01
#define ENV_SUSTAIN 0x02
#define ENV_RELEASE 0x03

__idata static uint8_t v1_env_state    = 0;
__idata static uint8_t v1_env_decay    = 0;
__idata static uint8_t v1_env_sust_vol = 0;

void update_envelope(void)
{
	++v1_env1_cnt;
	if (v1_env1_cnt >= v1_env1_period) {
		v1_env1_cnt = 0;
		if (v1_env_state > 0) {
			if (v1_env_state == ENV_ATTACK) {
				++v1_env_volume;
				if (v1_env_volume == 0xFF) {
					v1_env_state   = ENV_SUSTAIN;
					v1_env1_period = v1_env_decay;
				}
			} else {
				++v1_env2_cnt;
				if (v1_env2_cnt >= v1_env2_period) {
					v1_env2_cnt = 0;
					if ((v1_env_sust_vol != v1_env_volume) || 
		                (v1_env_state == ENV_RELEASE))
						--v1_env_volume;
					if (v1_env_volume == 0) {
						v1_env2_period = 1;
						v1_env_state  = 0;
					} else 
						v1_env2_period =
							(v1_env_volume < 55
		                        ? (v1_env_volume < 15
		                            ? (v1_env_volume < 7
		                                ? 30
		                                : 16)
		                            : v1_env_volume < 27
		                                ? 8
		                                : 4)
		                        : v1_env_volume < 94
		                            ? 2
		                            : 1);
				}
			}
		}
	}
}


void imme_sound_init(void)
{
    int i;

    // Use timer 4 for ~25.3kHz PWM output on P1.0
    //
    P1DIR  |= 0x01;
    T4CTL   = 0x50; // prescale, free-running
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

__xdata static uint8_t playSnd = 0;

__idata static uint16_t sndCnt = 0;

void imme_breathe_trigger(void)
{
    if (sndCnt < 40L * 256)
        v1_freq += 1;
    else if (sndCnt == 60L * 256)
        v1_env_state = ENV_RELEASE;
    else if (sndCnt == 80L * 256) {
        v1_freq  = 15000;
        v1_env1_period = 20;
        v1_env_decay = 30;
        v1_env_sust_vol = 0;
        v1_env_state = ENV_ATTACK;
    } else if (sndCnt < 180L * 256)
        v1_freq -= 200 + rand&255;
    else if (sndCnt == 180L * 256)
        v1_env_state = ENV_RELEASE;
    else if (v1_env_state == 0) {
		playSnd = 0;
		return;
	}
    ++sndCnt;
}

typedef void (*SndProc)(void);

static SndProc sndProc = 0;


void imme_breathe(void)
{
	uint8_t EA_old = EA;
	EA = 0;
    v1_noise   = 1;
    v1_square  = 0;
    v1_tri     = 0;
    v1_saw     = 0;
    v1_freq  = 100;
    v1_env1_period = 40;
    v1_env_decay = 20;
    v1_env_sust_vol = 100;
	sndProc = imme_breathe_trigger;
	//playSnd = 1;
    v1_env_state = ENV_ATTACK;
	sndCnt = 0;
	EA = EA_old;
}


void imme_sound_trigger(void)
{
	uint8_t EA_old = EA;
	EA = 0;

	while (dma_buf0_lidx != dma_buf0_idx) {
		uint8_t val;
//		if (playSnd) {
		if (sndProc) {
			//imme_breathe_trigger();
			sndProc();
			update_voice();
			update_envelope();
			val = ((uint16_t)v1_out * (uint16_t)v1_env_volume) >> 8;
			//val = v1_out;
			if (val < 5) val = 5;
			if (val > 250) val = 250;
		} else {

			val = 5;
		}
		dma_buf0[dma_buf0_lidx] = val;
		dma_buf0_lidx = (dma_buf0_lidx + 1) & DMA_BUFMOD;
	}

	EA = EA_old;
}


