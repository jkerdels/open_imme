#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("Usage: %s pgm-input-gfx pgm-input-mask gfxname\n",argv[0]);
        return -1;
    }

    // input gfx file
    FILE *inFile = fopen(argv[1],"rb");
    if (!inFile) {
        printf("Could not open gfx-input file: %s\n",argv[1]);
        return -2;
    }

    // check pgm header P5
    uint16_t header = 0;
    fread(&header,1,2,inFile);
    if (header != 0x3550) {
        printf("Gfx-Input file is not a pgm-file!\n");
        fclose(inFile);
        return -4;
    }

    // find resolution
    int gfxwidth  = 0;
    int gfxheight = 0;
    int gfxmaxGrayValue = 0;
    // skip whitespaces or comment lines starting with "#"
    char buffer[10];
    memset(buffer,0,10);
    uint8_t curVal,rr;
    while (1) {
        do {
            rr = fread(&curVal,1,1,inFile);
        } while ((rr) && (isspace(curVal)));
        if (!rr) {
            printf("Gfx-Input unexpectetly short! Aborting.\n");
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
        printf("Gfx-Input unexpectetly short! Aborting.\n");
        fclose(inFile);
        return -6;
    }
    gfxwidth = atoi(buffer);
    // read height
    bufIdx = 0;
    memset(buffer,0,10);
    do {
        rr = fread(&curVal,1,1,inFile);
        buffer[bufIdx++] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Gfx-Input unexpectetly short! Aborting.\n");
        fclose(inFile);
        return -7;
    }
    gfxheight = atoi(buffer);
    // read maxGray
    bufIdx = 0;
    memset(buffer,0,10);
    do {
        rr = fread(&curVal,1,1,inFile);
        buffer[bufIdx++] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Gfx-Input unexpectetly short! Aborting.\n");
        fclose(inFile);
        return -8;
    }
    gfxmaxGrayValue = atoi(buffer);

    int gfxpixelSize = (gfxmaxGrayValue < 256) ? 1 : 2;



    // input mask file
    FILE *maskFile = fopen(argv[2],"rb");
    if (!inFile) {
        printf("Could not open mask-input file: %s\n",argv[2]);
        fclose(inFile);
        return -2;
    }

    // check pgm header P5
    header = 0;
    fread(&header,1,2,maskFile);
    if (header != 0x3550) {
        printf("Mask-Input file is not a pgm-file!\n");
        fclose(maskFile);
        fclose(inFile);
        return -4;
    }

    // find resolution
    int maskwidth  = 0;
    int maskheight = 0;
    int maskmaxGrayValue = 0;
    // skip whitespaces or comment lines starting with "#"
    memset(buffer,0,10);
    while (1) {
        do {
            rr = fread(&curVal,1,1,maskFile);
        } while ((rr) && (isspace(curVal)));
        if (!rr) {
            printf("Mask-Input unexpectetly short! Aborting.\n");
            fclose(maskFile);
            fclose(inFile);
            return -5;
        }
        if (curVal != '#') {
            buffer[0] = curVal;
            break;
        }
        // skip to newline
        do {
            rr = fread(&curVal,1,1,maskFile);
        } while ((rr) && (curVal != '\n'));
    }
    // read rest of number
    bufIdx = 0;
    do {
        rr = fread(&curVal,1,1,maskFile);
        buffer[++bufIdx] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Mask-Input unexpectetly short! Aborting.\n");
        fclose(maskFile);
        fclose(inFile);
        return -6;
    }
    maskwidth = atoi(buffer);
    // read height
    bufIdx = 0;
    memset(buffer,0,10);
    do {
        rr = fread(&curVal,1,1,maskFile);
        buffer[bufIdx++] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Mask-Input unexpectetly short! Aborting.\n");
        fclose(maskFile);
        fclose(inFile);
        return -7;
    }
    maskheight = atoi(buffer);
    // read maxGray
    bufIdx = 0;
    memset(buffer,0,10);
    do {
        rr = fread(&curVal,1,1,maskFile);
        buffer[bufIdx++] = curVal;
    } while ((rr) && (!isspace(curVal)));
    if (!rr) {
        printf("Mask-Input unexpectetly short! Aborting.\n");
        fclose(maskFile);
        fclose(inFile);
        return -8;
    }
    maskmaxGrayValue = atoi(buffer);

    int maskpixelSize = (maskmaxGrayValue < 256) ? 1 : 2;


    if ((gfxwidth != maskwidth) || (gfxheight != maskheight)) {
        printf("Gfx-Input and Mask-Input have different dimensions! Aborting.\n");
        fclose(maskFile);
        fclose(inFile);
        return -8;
    }

    // read to read the stuff ;-)
    int dataSize  = gfxwidth * gfxheight;
    uint8_t *data = malloc(dataSize);
    int i;
    for (i = 0; i < dataSize; ++i) {
        uint16_t pixelVal = 0;
        fread(&pixelVal,gfxpixelSize,1,inFile);
        if (pixelVal < (gfxmaxGrayValue / 4))
            data[i] = 3;
        else if (pixelVal < (gfxmaxGrayValue / 2))
            data[i] = 2;
        else if (pixelVal < (gfxmaxGrayValue * 3 / 4))
            data[i] = 1;
        else
            data[i] = 0;
        fread(&pixelVal,maskpixelSize,1,maskFile);
        if (pixelVal > (maskmaxGrayValue / 2))
            data[i] |= 4;
    }

    // generate output
    char defBuf[256];
    memset(defBuf,0,256);
    i = 0;
    while((argv[3][i] != 0) && (i < 253)) {
        defBuf[i] = toupper(argv[3][i]);
        ++i;
    }
    defBuf[i++] = '_';
    defBuf[i] = 'H';

    printf("#ifndef %s\n#define %s\n\n#include <stdint.h>\n\n",defBuf,defBuf);

    printf("__xdata const uint8_t %sWidth  = %i;\n\n",argv[3],gfxwidth);
    printf("__xdata const uint8_t %sHeight = %i;\n\n",argv[3],gfxheight);

    printf("__code const uint8_t %s[] = {\n",argv[3]);

    int x,page;

    for (page = 0; page < (gfxheight / 8); ++page) {
        printf("    ");
        for (x = 0; x < gfxwidth; ++x) {
            uint8_t curCol = 0;
            for (i = 0; i < 8; ++i)
                curCol |= (data[(((page*8)+i)*gfxwidth)+x] & 1) << i;
            printf("0x%X, ",curCol);
        }
        printf(" // LSB\n");

        printf("    ");
        for (x = 0; x < gfxwidth; ++x) {
            uint8_t curCol = 0;
            for (i = 0; i < 8; ++i)
                curCol |= ((data[(((page*8)+i)*gfxwidth)+x] & 2) >> 1) << i;
            printf("0x%X, ",curCol);
        }
        printf(" // MSB\n");

        printf("    ");
        for (x = 0; x < gfxwidth; ++x) {
            uint8_t curCol = 0;
            for (i = 0; i < 8; ++i)
                curCol |= ((data[(((page*8)+i)*gfxwidth)+x] & 4) >> 2) << i;
            printf("0x%X, ",curCol);
        }
        printf(" // MASK\n");
    }
    // remaining lines
    page = gfxheight / 8;
    uint8_t gfxrem = gfxheight % 8;
    if (gfxrem) {
        printf("    ");
        for (x = 0; x < gfxwidth; ++x) {
            uint8_t curCol = 0;
            for (i = 0; i < gfxrem; ++i)
                curCol |= (data[(((page*8)+i)*gfxwidth)+x] & 1) << i;
            printf("0x%X, ",curCol);
        }
        printf(" // LSB\n");

        printf("    ");
        for (x = 0; x < gfxwidth; ++x) {
            uint8_t curCol = 0;
            for (i = 0; i < gfxrem; ++i)
                curCol |= ((data[(((page*8)+i)*gfxwidth)+x] & 2) >> 1) << i;
            printf("0x%X, ",curCol);
        }
        printf(" // MSB\n");

        printf("    ");
        for (x = 0; x < gfxwidth; ++x) {
            uint8_t curCol = 0;
            for (i = 0; i < gfxrem; ++i)
                curCol |= ((data[(((page*8)+i)*gfxwidth)+x] & 4) >> 2) << i;
            printf("0x%X, ",curCol);
        }
        printf(" // MASK\n");
    }

    printf("};\n\n");

    printf("#endif\n");

    free(data);

    fclose(inFile);
    fclose(maskFile);

    return 0;
}
