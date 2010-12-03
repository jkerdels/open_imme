#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <util/delay.h>
#include "usb_serial.h"

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

#define DBG_DDR  DDRF
#define DBG_PORT PORTF
#define RST      16
#define DATA_OUT 2
#define DATA_IN  32
#define DB_CLK   1
#define US_DELAY 1

#define FLASHPAGE_SIZE       1024
#define FLASH_WORD_SIZE         2
#define WORDS_PER_FLASH_PAGE  512

void init(void) 
{
	//DBG_PORT = 0x00;

	// reset low
	DBG_PORT &= ~RST;
	_delay_us(US_DELAY);

	// dbg clock high
	DBG_PORT ^= DB_CLK;
	_delay_us(US_DELAY);
	// dbg clock low
	DBG_PORT ^= DB_CLK;
	_delay_us(US_DELAY);
	// dbg clock high
	DBG_PORT ^= DB_CLK;
	_delay_us(US_DELAY);
	// dbg clock low
	DBG_PORT ^= DB_CLK;

	// reset high
	_delay_us(US_DELAY);
	DBG_PORT |= RST;

}

uint8_t send_byte(uint8_t data)
{
	for (int j = 0; j < 8; ++j) {

		if (data & 0x80)
			DBG_PORT |= DATA_OUT;
		else
			DBG_PORT &= ~DATA_OUT;

		data <<= 1;

		_delay_us(US_DELAY);
		// dbg clock high
		DBG_PORT |= DB_CLK;

		_delay_us(US_DELAY);

		// dbg clock low
		DBG_PORT &= ~DB_CLK;

		// sample
		if (PINF & DATA_IN)
			data |= 1;
	}
	return data;	
}



void chip_erase(void)
{
	send_byte(0x14);
	send_byte(0); // ignore sent value
}

void write_config(uint8_t cfg)
{
	send_byte(0x1D);	
	send_byte(cfg & 0x0F);	
	send_byte(0); // ignore sent value
}

// status bits received by read_status()
#define ST_CHIP_ERASE_DONE   0x80
#define ST_PCON_IDLE         0x40
#define ST_CPU_HALTED        0x20
#define ST_POWER_MODE_0      0x10
#define ST_HALT_STATUS       0x08
#define ST_DEBUG_LOCKED      0x04
#define ST_OSCILLATOR_STABLE 0x02
#define ST_STACK_OVERFLOW    0x01

uint8_t read_status(void)
{
	send_byte(0x34);
	return send_byte(0);
}

uint16_t get_chip_id(void)
{
	send_byte(0x68);
	uint16_t result = send_byte(0);
	result |= send_byte(0) << 8;
	return result;
}

void cpu_halt(void)
{
	send_byte(0x44);
	send_byte(0); // ignore sent value
}

void cpu_resume(void)
{
	send_byte(0x4C);
	send_byte(0); // ignore sent value
}

uint8_t debug_instr_1(uint8_t in0)
{
	send_byte(0x55);	
	send_byte(in0);	
	return send_byte(0);
}

uint8_t debug_instr_2(uint8_t in0, uint8_t in1)
{
	send_byte(0x56);	
	send_byte(in0);	
	send_byte(in1);	
	return send_byte(0);
}

uint8_t debug_instr_3(uint8_t in0, uint8_t in1, uint8_t in2)
{
	send_byte(0x57);	
	send_byte(in0);	
	send_byte(in1);	
	send_byte(in2);	
	return send_byte(0);
}

void read_code_memory(uint16_t address, 
                      uint8_t  bank, 
                      uint16_t count, 
                      uint8_t *outputData)
{
	if (address >= 0x8000)
		address = (address & 0x7FFF) + (bank * 0x8000);

	debug_instr_3(0x75,0xC7,(bank * 16) + 1);
	debug_instr_3(0x90,address >> 8,address);
	for (int i = 0; i < count; ++i) {
		debug_instr_1(0xE4);
		outputData[i] = debug_instr_1(0x93);
		debug_instr_1(0xA3);
	}
}

void read_xcode_memory(uint16_t address, 
                       uint16_t count, 
                       uint8_t *outputData)
{
	debug_instr_3(0x90,address >> 8,address);
	for (int i = 0; i < count; ++i) {
		outputData[i] = debug_instr_1(0xE0);
		debug_instr_1(0xA3);
	}	
}

void write_xdata_memory(uint16_t address, 
                        uint16_t count, 
                        uint8_t *inputData)
{
	debug_instr_3(0x90,address >> 8,address);
	for (int i = 0; i < count; ++i) {
		debug_instr_2(0x74,inputData[i]);
		debug_instr_1(0xF0);
		debug_instr_1(0xA3);	
	}
}

void set_pc(uint16_t address)
{
	debug_instr_3(0x02,address >> 8,address);
}

void clock_init(void)
{
	debug_instr_3(0x75,0xC6,0x00);
	uint8_t sleepReg;
	do {
	    _delay_us(1000);
		sleepReg = debug_instr_2(0xE5,0xBE);
	} while (!(sleepReg & 0x40));
}

void write_flash_page(uint32_t address, 
                      uint8_t *inputData,
					  uint8_t  erase_page)
{
	uint8_t updProc[] = { 0x75, 0xAD, ((address >> 8) / FLASH_WORD_SIZE) & 0x7E,
                          0x75, 0xAC, 0x00, 
						  0x75, 0xAB, 0x23, 0x00,
                          0x75, 0xAE, 0x01, // ------
                          0xE5, 0xAE,       // erase code
                          0x20, 0xE7, 0xFB, // ------
                          0x90, 0xF0, 0x00,
                          0x7F, WORDS_PER_FLASH_PAGE >> 8,
                          0x7E, WORDS_PER_FLASH_PAGE & 0xFF,
                          0x75, 0xAE, 0x02,
                          0x7D, FLASH_WORD_SIZE,
                          0xE0, 
                          0xA3, 
                          0xF5, 0xAF,
                          0xDD, 0xFA,
                          0xE5, 0xAE,
                          0x20, 0xE6, 0xFB,
                          0xDE, 0xF1,
                          0xDF, 0xEF,
                          0xA5
	};

	uint8_t updProcSize = sizeof(updProc);

	// remove code section for erase from updProc
/*
	if (!erase_page) {
		for (int i = 14; i < updProcSize; ++i)
			updProc[i-8] = updProc[i];
		updProcSize -= 8;
	}
*/
	write_xdata_memory(0xF000,  FLASHPAGE_SIZE, inputData);
    write_xdata_memory(0xF000 + FLASHPAGE_SIZE, updProcSize, updProc);
	debug_instr_3(0x75,0xC7,0x51);
	set_pc(0xF000 + FLASHPAGE_SIZE);
	cpu_resume();
	uint8_t status;
	do {
		_delay_ms(100);
		status = read_status();
	} while (!(status & ST_CPU_HALTED));
}


void read_flash_page(uint32_t address, uint8_t *outputData)
{
	read_code_memory(address & 0xFFFF, 
                     (address >> 15) & 0x03, FLASHPAGE_SIZE, outputData);
}

void mass_erase_flash(void)
{
	debug_instr_1(0x00);
	chip_erase();
	uint8_t status = 0;
	do {
	    _delay_ms(100);
		status = read_status();
	} while (!(status & ST_CHIP_ERASE_DONE));
}


#define STATUS_CMD 0x01
#define CPUID_CMD  0x02
#define ERASE_CMD  0x04
#define PROG_CMD   0x08

int main(void)
{
	// set for 16 MHz clock
	CPU_PRESCALE(1);

	// initialize the USB, and then wait for the host
	// to set configuration.  If the Teensy is powered
	// without a PC connected to the USB port, this 
	// will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;
	_delay_ms(100);

	int16_t r;
	uint8_t cmd;

	DBG_DDR = RST | DB_CLK | DATA_OUT;
	DBG_PORT = RST;

	while (1) {
		// read command
		r = usb_serial_getchar();		
		if (r != -1) {
			init();
			clock_init();
			cmd = r;
			if (cmd & STATUS_CMD) {
				uint8_t status = read_status();
				do {
					r = usb_serial_putchar(status);
				} while (r == -1);
			}
			if (cmd & CPUID_CMD) {
				uint16_t chipInfo = get_chip_id();
				do {
					r = usb_serial_putchar(chipInfo & 0xFF);
				} while (r == -1);
				do {
					r = usb_serial_putchar((chipInfo >> 8) & 0xFF);
				} while (r == -1);
			}
			if (cmd & ERASE_CMD) {
				mass_erase_flash();
				do {
					r = usb_serial_putchar('A');
				} while (r == -1);
			}
			if (cmd & PROG_CMD) {
				
				uint8_t pageBuf[FLASHPAGE_SIZE];

				uint8_t i = 0;
				while (1) {
					r = usb_serial_getchar();
					if (r == 0xFF) {
						do {
							r = usb_serial_putchar(0xFF);
						} while (r == -1);
						for (int j = 0; j < FLASHPAGE_SIZE; ++j) {
							do {
								r = usb_serial_getchar();
							} while (r == -1);
							pageBuf[j] = r;
						}
						uint32_t pageAddr = (uint32_t)i * (uint32_t)FLASHPAGE_SIZE;
						write_flash_page(pageAddr,pageBuf,0);
						read_flash_page(pageAddr,pageBuf);
						for (int j = 0; j < FLASHPAGE_SIZE; ++j) {
							do {
								r = usb_serial_putchar(pageBuf[j]);
							} while (r == -1);
						}
						do {
							r = usb_serial_putchar(0x00);
						} while (r == -1);
						i++;
					} else if (r == 0xF0) {
						--i;
					} else if (r == 0x11) {
						set_pc(0xF000);
						cpu_resume();
						break;
					}
				}
			
			}
		}
	}
}




