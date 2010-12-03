#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define BUFFERSIZE 256

#define STATUS_CMD 0x01
#define CPUID_CMD  0x02
#define ERASE_CMD  0x04
#define PROG_CMD   0x08

#define FLASHPAGE_SIZE 1024

#define FLASHSIZE 0x8000

// reads hex file into memory
// on error returns negative value
// on success returns highest address used + 1
int32_t readHex(FILE *inputfile, uint8_t *outputData, uint32_t outputDataSize);

int main(int argc, char **argv)
{
    uint8_t errorParsing = 0;

    uint8_t cmd = 0;

    char serialPort[BUFFERSIZE];
    memset(serialPort,0,BUFFERSIZE);
    char hexFile[BUFFERSIZE];
    memset(hexFile,0,BUFFERSIZE);

    int i = 1;
    while (i < argc) {
        if (argv[i][0] == '-')
            switch (argv[i][1]) {
                case 'e' : { // set erase flag
                    cmd |= ERASE_CMD;
                } break;
                case 'P' : { // serial port to use
                    if (i+1 < argc) {
                        strncat(serialPort,argv[++i],BUFFERSIZE-1);
                    } else {
                        errorParsing = 1;
                    }
                } break;
                case 'f' : { // hex file to flash
                    if (i+1 < argc) {
                        cmd |= PROG_CMD;
                        strncat(hexFile,argv[++i],BUFFERSIZE-1);
                    } else {
                        errorParsing = 1;
                    }
                } break;
                case 's' : { // read status
                    cmd |= STATUS_CMD;
                } break;
                case 'c' : { // read cpu id
                    cmd |= CPUID_CMD;
                } break;
                default : {
                    errorParsing = 1;
                } break;
            }
        ++i;
    }

    // trying to open hex file (if specified)
    FILE *hexFD = 0;
    if (hexFile[0] != 0) {
        hexFD = fopen(hexFile,"rb");
        if (hexFD == 0) {
            printf("unable to open hex file");
            errorParsing = 1;
        }
    }

    // trying to open serial port
    int serialFD = -1;

	// open serial port
	serialFD = open(serialPort, O_RDWR | O_NOCTTY);
	if (serialFD == -1) {
		printf("unable to open serial port");
		errorParsing = 1;
	}

	// setting serial to non-blocking
	//int x = fcntl(serialFD, F_GETFL, 0);
	//fcntl(serialFD, F_SETFL, x | O_NONBLOCK);

	struct termios serialPortOptions;
	// fill structure
	tcgetattr(serialFD, &serialPortOptions);
	// set baud rate
	cfsetispeed(&serialPortOptions, B115200);
	cfsetospeed(&serialPortOptions, B115200);
	// set 8n1
	serialPortOptions.c_cflag &= ~PARENB;
	serialPortOptions.c_cflag &= ~CSTOPB;
	serialPortOptions.c_cflag &= ~CSIZE;
	serialPortOptions.c_cflag |= CS8;
	// set local, read, write, etc
	serialPortOptions.c_cflag |= (CLOCAL | CREAD);
	// set for RAW input
	serialPortOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// disable software flow control
	serialPortOptions.c_iflag &= ~(IXON | IXOFF | IXANY);
	// set to RAW output
	serialPortOptions.c_oflag &= ~OPOST;
	// set config now
	tcsetattr(serialFD, TCSANOW, &serialPortOptions);


    // if there was an error or insufficient parameters
    // exit with printing usage info
    if (errorParsing) {
        printf("Usage:\n\n %s [OPTIONS]\n\nOptions:\n",argv[0]);
        printf("    -s\n       get status\n");
        printf("    -c\n       get chip id\n");
        printf("    -e\n       perform flash erase\n");
        printf("    -P SERIALDEVICE\n       serial port to use, e.g. /dev/ttyACM0\n");
        printf("    -f HEXFILE\n       hexfile to flash\n\n");
        return -1;
    }

    // send commands to teensy
    write(serialFD,&cmd,1);

    char rcvBuf[BUFFERSIZE];
    memset(rcvBuf,0,BUFFERSIZE);

    if (cmd & STATUS_CMD) {
        // read status
        uint8_t status = 0;
        printf("----\nreading status:\n");
        read(serialFD,&status,1);
        printf("CHIP_ERASE_DONE:   %u\n"
               "PCON_IDLE:         %u\n"
               "CPU_HALTED:        %u\n"
               "POWER_MODE_0:      %u\n"
               "HALT_STATUS:       %u\n"
               "DEBUG_LOCKED:      %u\n"
               "OSCILLATOR_STABLE: %u\n"
               "STACK_OVERFLOW:    %u\n\n",
                (status >> 7) & 1,
                (status >> 6) & 1,
                (status >> 5) & 1,
                (status >> 4) & 1,
                (status >> 3) & 1,
                (status >> 2) & 1,
                (status >> 1) & 1,
                status & 1);
    }

    if (cmd & CPUID_CMD) {
        // read CPU ID
        uint8_t cpuid = 0;
        uint8_t cpuver = 0;
        printf("----\nreading chip id:\n");
        read(serialFD,&cpuid,1);
        printf("CHIPID:  %u\n",cpuid);
        read(serialFD,&cpuver,1);
        printf("CHIPVER: %u\n\n",cpuver);
    }

    if (cmd & ERASE_CMD) {
        // just wait for ack
        printf("----\nerasing flash\n");
        uint8_t ack = 0;
        read(serialFD,&ack,1);
        printf("DONE\n");
    }

    if (cmd & PROG_CMD) {
        // write hex file to flasher
        //FILE *rawout = fopen("rawout.dat","wb");

        // read hex file into temp-array
        uint8_t inputData[FLASHSIZE];
        memset(inputData,0xFF,FLASHSIZE);
        int32_t inputDataSize;
        int32_t inputDataIdx = 0;
        inputDataSize = readHex(hexFD,inputData,FLASHSIZE);
        if (inputDataSize < 0) {
            printf("ERROR IN HEX FILE!\n");
            if (serialFD > 0)
                close(serialFD);
            if (hexFD)
                fclose(hexFD);
            return -1;
        }

        printf("----\nwriting flash\n");
        uint8_t rbuf[FLASHPAGE_SIZE];
        uint8_t cbuf[FLASHPAGE_SIZE];
        uint8_t repeat = 0;
        uint8_t pageIdx = 0;
        int nrOfBytes = 0;
        while (inputDataIdx < inputDataSize) {
            if (!repeat) {
                memset(rbuf,0xFF,FLASHPAGE_SIZE);
                if (inputDataIdx + FLASHPAGE_SIZE <= inputDataSize)
                    nrOfBytes = FLASHPAGE_SIZE;
                else
                    nrOfBytes = inputDataSize - inputDataIdx;
                memcpy(rbuf,&(inputData[inputDataIdx]),nrOfBytes);
                inputDataIdx += nrOfBytes;
                pageIdx++;
            } else {
                uint8_t cmd = 0xF0;
                write(serialFD,&cmd,1);
            }
            if (nrOfBytes) {
                uint8_t cmd = 0xFF;
                write(serialFD,&cmd,1);
                cmd = 0x00;
                read(serialFD,&cmd,1);
                if (cmd == 0xFF) {
                    printf("writing page %u\n",pageIdx);
                    write(serialFD,rbuf,FLASHPAGE_SIZE);
                    printf("reading back\n");
                    uint8_t *curData = cbuf;
                    int bytesRead = 0;
                    while (bytesRead < FLASHPAGE_SIZE) {
                        int curRead = read(serialFD,curData,FLASHPAGE_SIZE-bytesRead);
                        curData  += curRead;
                        bytesRead += curRead;
                    }
                    int i;
                    repeat = 0;

                    for (i = 0; i < FLASHPAGE_SIZE; ++i)
                        if (cbuf[i] != rbuf[i]) {
                            printf("error->repeating page\n");
                            repeat = 1;
                        }
                        /*
                    if (!repeat)
                        fwrite(cbuf,1,FLASHPAGE_SIZE,rawout);
                        */
                    printf("awaiting ack\n");
                    read(serialFD,&cmd,1);
                    if (cmd != 0x00){
                        printf("ERROR: did not receive write ack\n");
                        break;
                    }
                } else {
                    printf("ERROR: did not receive ready ack\n");
                    break;
                }
            }
        }
        printf("DONE\n");
        uint8_t cmd = 0x11;
        write(serialFD,&cmd,1);
        //fclose(rawout);
    }

    if (serialFD > 0)
        close(serialFD);

    if (hexFD)
        fclose(hexFD);

    printf("bye bye...\n");

    return 0;
}


// reads hex file into memory
// on error returns negative value
// on success returns highest address used + 1
#define MEMSTEP   1024
#define RBUF_SIZE 4096

uint8_t hex2int(uint8_t hVal)
{
    if (hVal < '0')
        printf("hval wrong: %u\n",hVal);
    if (hVal - '0' < 10)
        return hVal - '0';
    if (hVal - 'A' < 6)
        return hVal -'A' + 10;
    if (hVal - 'a' < 6)
        return hVal -'a' + 10;
    return 16;
}

int32_t readHex(FILE *inputfile, uint8_t *outputData, uint32_t outputDataSize)
{
    uint8_t scanstate = 0;

    uint8_t  recordSize = 0;
    uint16_t recordAddr = 0;
    uint8_t  recordType = 0;
    uint8_t  recordData[256];
    memset(recordData,0,256);
    uint8_t  recordDataIdx = 0;
    uint8_t  checksum = 0;
    uint8_t  recordCheck   = 0;

    uint32_t extSegAddr  = 0;
    uint32_t linBaseAddr = 0;

    uint32_t maxAddrUsed = 0;

    while (1) {
        uint8_t  readBuffer[RBUF_SIZE];
        uint32_t nrOfBytes = fread(readBuffer,1,RBUF_SIZE,inputfile);
        uint8_t  needMoreData = 0;
        uint32_t  readIdx = 0;
        while (!needMoreData) {
            switch (scanstate) {
                // search for ":", 0x3A
                case 0 : {
                    while ((readIdx < nrOfBytes) && (readBuffer[readIdx] != 0x3A))
                        readIdx++;
                    if (readIdx < nrOfBytes) {
                        readIdx++;
                        scanstate = 1;
                        memset(recordData,0,256);
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get high nibble of size
                case 1 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in high nibble of size!\n");
                        recordSize = (val & 0x0F) << 4;
                        scanstate = 2;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get low nibble of size
                case 2 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in low nibble of size!\n");
                        recordSize |= (val & 0x0F);
                        recordCheck = recordSize;
                        scanstate = 3;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get high nibble of high byte of address
                case 3 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in high nibble of high byte of address!\n");
                        recordAddr = (val & 0x0F) << 12;
                        scanstate = 4;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get low nibble of high byte of address
                case 4 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in low nibble of high byte of address!\n");
                        recordAddr |= (val & 0x0F) << 8;
                        recordCheck += (recordAddr >> 8) & 0xFF;
                        scanstate = 5;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get high nibble of low byte of address
                case 5 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in high nibble of low byte of address!\n");
                        recordAddr |= (val & 0x0F) << 4;
                        scanstate = 6;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get low nibble of low byte of address
                case 6 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in low nibble of low byte of address!\n");
                        recordAddr |= (val & 0x0F);
                        recordCheck += recordAddr & 0xFF;
                        scanstate = 7;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get high nibble of type
                case 7 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in high nibble of type!\n");
                        recordType = (val & 0x0F) << 4;
                        scanstate = 8;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get low nibble of type
                case 8 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in low nibble of type!\n");
                        recordType |= (val & 0x0F);
                        recordCheck += recordType;
                        recordDataIdx = 0;
                        scanstate = 9;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get high nibble of data
                case 9 : {
                    if (recordDataIdx < recordSize) {
                        if (readIdx < nrOfBytes) {
                            uint8_t dval = readBuffer[readIdx++];
                            uint8_t val = hex2int(dval);
//                            if (recordAddr == 164)
//                                printf("DH:%u -> %u, Ri:%u\n",dval,val, readIdx);
                            if (val > 15)
                                printf("Warning: no hex value in high nibble of data: %u, %u!\n",val,recordDataIdx);
                            recordData[recordDataIdx] = (val & 0x0F) << 4;
                            scanstate = 10;
                        } else {
                            needMoreData = 1;
                        }
                    } else {
                        scanstate = 11;
                    }
                } break;
                // get low nibble of data
                case 10 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t dval = readBuffer[readIdx++];
                        uint8_t val = hex2int(dval);
//                        if (recordAddr == 164)
//                            printf("DL:%u -> %u\n",dval,val);
                        if (val > 15)
                            printf("Warning: no hex value in low nibble of data!\n");
                        recordData[recordDataIdx] |= (val & 0x0F);
                        recordCheck += recordData[recordDataIdx++];
                        scanstate = 9;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get high nibble of checksum
                case 11 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in high nibble of checksum!\n");
                        checksum = (val & 0x0F) << 4;
                        scanstate = 12;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // get low nibble of checksum
                case 12 : {
                    if (readIdx < nrOfBytes) {
                        uint8_t val = hex2int(readBuffer[readIdx++]);
                        if (val > 15)
                            printf("Warning: no hex value in low nibble of checksum!\n");
                        checksum |= (val & 0x0F);
                        recordCheck += checksum;
                        if (recordCheck != 0) {
                            printf("Checksum Error! %u, %u, %u, %u, %u\n",recordCheck,recordAddr,recordType,recordSize, recordDataIdx);
                            int k;
                            for (k = 0; k < recordSize; ++k)
                                printf("%u, ", recordData[k]);
                            printf("\n");
                            return -1;
                        }
                        // we have a good record, check type
                        scanstate = 13 + recordType;
                    } else {
                        needMoreData = 1;
                    }
                } break;
                // we have a data record
                case 13 : {
                    uint32_t targetAddr = linBaseAddr | (extSegAddr + recordAddr);
                    if (targetAddr + recordSize <= outputDataSize) {
                        memcpy(outputData + targetAddr,recordData,recordSize);
                        if (maxAddrUsed < targetAddr + recordSize)
                            maxAddrUsed = targetAddr + recordSize;
                    } else {
                        printf("Error: target address to big\n");
                        return -3;
                    }
                    scanstate  = 0;
                } break;
                // we have a End Of File Record
                case 14 : {
                    return maxAddrUsed;
                } break;
                // we have a Extended Segment Address Record
                case 15 : {
                    extSegAddr = (recordData[0] << 8 | recordData[1]) * 16;
                    scanstate  = 0;
                } break;
                // we have a Start Segment Address Record
                case 16 : {
                    printf("Warning: Start Segment Address Records not supported\n");
                    scanstate  = 0;
                } break;
                // we have a Extended Linear Address Record
                case 17 : {
                    linBaseAddr = (recordData[0] << 24) | (recordData[1] << 16);
                    scanstate   = 0;
                } break;
                // we have a Start Linear Address Record.
                case 18 : {
                    printf("Warning: Start Linear Address Records not supported\n");
                    scanstate  = 0;
                } break;
                // unknown state.. should not happen
                default : {
                    printf("Unknwon scanstate %i!\n",scanstate);
                    return -2;
                } break;
            } // endswitch
        } // endwhile !needMoreData
    } // endwhile 1

    return 0;
}

