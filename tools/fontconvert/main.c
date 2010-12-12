#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s pgm-input-file fontname\n",argv[0]);
        return -1;
    }

    FILE *inFile = fopen(argv[1],"rb");
    if (!inFile) {
        printf("Could not open input file: %s\n",argv[1]);
        return -2;
    }

    // check pgm header P5
    uint16_t header = 0;
    fread(&header,1,2,inFile);
    if (header != 0x3550) {
        printf("Input file is not a pgm-file!\n");
        fclose(inFile);
        return -4;
    }

    // find resolution
    int width  = 0;
    int charWidth = 0;
    int height = 0;
    int maxGrayValue = 0;
    // skip whitespaces or comment lines starting with "#"
    char buffer[10];
    memset(buffer,0,10);
    uint8_t curVal,rr;
    while (1) {
        do {
            rr = fread(&curVal,1,1,inFile);
        } while ((rr) && (isspace(curVal)));
        if (!rr) {
            printf("Input unexpectetly short! Aborting.\n");
            fclose(inFile);
            return -5;
        }
        if (curVal != '#') {
            buffer[0] = curVal;
            break;
        }
        // skip to newline
        do {
            rr = fread(&curVal,1,1,inFile);
        } while ((rr) && (curVal != '\n'));
    }
    // read rest of number
    int bufIdx = 0;
    do {
        rr = fread(&curVal,1,1,inFile);
        buffer[++bufIdx] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Input unexpectetly short! Aborting.\n");
        fclose(inFile);
        return -6;
    }
    width = atoi(buffer);
    charWidth = width / 94;
    if (width % 94 != 0) {
        printf("Width (%i) of input is no multiple of 94(chars)! Aborting.\n",width);
        fclose(inFile);
        return -9;
    }
    // read height
    bufIdx = 0;
    memset(buffer,0,10);
    do {
        rr = fread(&curVal,1,1,inFile);
        buffer[bufIdx++] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Input unexpectetly short! Aborting.\n");
        fclose(inFile);
        return -7;
    }
    height = atoi(buffer);
    if (height > 8) {
        printf("Heights > 8 currently not supported! Aborting.\n");
        fclose(inFile);
        return -10;
    }
    // read maxGray
    bufIdx = 0;
    memset(buffer,0,10);
    do {
        rr = fread(&curVal,1,1,inFile);
        buffer[bufIdx++] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Input unexpectetly short! Aborting.\n");
        fclose(inFile);
        return -8;
    }
    maxGrayValue = atoi(buffer);

    int pixelSize = (maxGrayValue < 256) ? 1 : 2;

    // ready to read the stuff ;-)
    int dataSize  = width * height;
    uint8_t *data = malloc(dataSize);
    int i,j,k;
    for (i = 0; i < dataSize; ++i) {
        uint16_t pixelVal = 0;
        fread(&pixelVal,pixelSize,1,inFile);
        if (pixelVal < (maxGrayValue / 4))
            data[i] = 3;
        else if (pixelVal < (maxGrayValue / 2))
            data[i] = 2;
        else if (pixelVal < (maxGrayValue * 3 / 4))
            data[i] = 1;
        else
            data[i] = 0;
    }

    // generate output
    char defBuf[256];
    memset(defBuf,0,256);
    i = 0;
    while((argv[2][i] != 0) && (i < 253)) {
        defBuf[i] = toupper(argv[2][i]);
        ++i;
    }
    defBuf[i++] = '_';
    defBuf[i] = 'H';


    printf("#ifndef %s\n#define %s\n\n#include <stdint.h>\n\n",defBuf,defBuf);

    printf("__code const uint8_t %s[] = {\n",argv[2]);

    printf("    0x%X, // font char width\n",charWidth);

    uint8_t offset = 8 - height;

    for (i = 0; i < 93; ++i) {
        printf("    ");
        for (j = 0; j < charWidth; ++j) {
            uint8_t curCol = 0;
            for (k = 0; k < height; ++k) {
                curCol |= (data[(i*charWidth)+j+(k*width)] & 1) << (k + offset);
            }
            printf("0x%X, ",curCol);
        }
        printf(" // %c LSB\n",i+33);
        printf("    ");
        for (j = 0; j < charWidth; ++j) {
            uint8_t curCol = 0;
            for (k = 0; k < height; ++k) {
                curCol |= ((data[(i*charWidth)+j+(k*width)] & 2) >> 1) << (k + offset);
            }
            printf("0x%X, ",curCol);
        }
        printf(" // %c MSB\n\n",i+33);
    }

    printf("};\n\n");

    printf("#endif\n");

    free(data);

    fclose(inFile);

    return 0;
}
