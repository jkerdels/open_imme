#include "sound.h"
#include "tools.h"

#define DMA_BUFLEN  128L

static __xdata uint8_t  dma_buf0[DMA_BUFLEN];

void sound_init(void)
{
    int i;

    // Use timer 4 for ~25.3kHz PWM output on P1.0
    //
    P1DIR  |= 0x01;
    T4CTL   = 0x30; // prescale, free-running
    T4CCTL0 = 0x24; // clear on compare, set on 0
    T4CCTL1 = 0x14; // toggle on compare (dma trigger)

    for (i=0; i<128; i++)
        dma_buf0[i] = 0;

    DMA_DESC[DMA_SOUND].SRCADDR  = BSWAP((uint16_t)dma_buf0);
    DMA_DESC[DMA_SOUND].DESTADDR = BSWAP((uint16_t)&X_T4CC0);
    DMA_DESC[DMA_SOUND].LENGTH   = BSWAP(DMA_BUFLEN);
    DMA_DESC[DMA_SOUND].FLAGS    = BSWAP(0x4A40);

    // Arm DMA channel
    //
    DMAARM |= 0x04;
}
