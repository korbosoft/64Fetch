#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c64/vic.h>
#include "detect.h"

#define clrstr(str) memset(str, '\0', sizeof(str))

char nybbleToHexChar(char nybble) {
    if (nybble < 10) {
        return nybble + '0';
    } else {
        return nybble - 10 + 'A';
    }
}

void print_crc32(unsigned long crc) {
    putchar(nybbleToHexChar((crc >> 28) & 0x0F));
    putchar(nybbleToHexChar((crc >> 24) & 0x0F));
    putchar(nybbleToHexChar((crc >> 20) & 0x0F));
    putchar(nybbleToHexChar((crc >> 16) & 0x0F));
    putchar(nybbleToHexChar((crc >> 12) & 0x0F));
    putchar(nybbleToHexChar((crc >> 8) & 0x0F));
    putchar(nybbleToHexChar((crc >> 4) & 0x0F));
    putchar(nybbleToHexChar(crc & 0x0F));
}

int main(void) {
    // char cpu[10];
    // char cpuResult;
    char dev8[36], dev9[36], dev10[36], dev11[36];
    char divLength;
    char i;
    char kernal[16];
    char kernalResult;
    unsigned long kernalCRC;
    char key;
    char model[17];
    char region[9];
    char regionResult;
    char sid[9];
    char sidResult;

    *(char *)0x0289 = 1;
    *(char *)0x028A = 0x7F;
    *(char *)0x0291 = 0xFF;

    clrscr();
    iocharmap(IOCHM_PETSCII_2);
    vic.color_border = 1;
    vic.color_back = 0;

    init_crc_tables();
    printf("\5SID...");
    sidResult = detect_sid();
    switch (sidResult) {
        case 1:
            strcpy(sid, "MOS 6581");
            break;
        case 2:
            strcpy(sid, "MOS 8580");
            break;
        default:
            strcpy(sid, "Unknown");
    }

    puts(" Done.\nRegion...");
    regionResult = detect_region();
    switch (regionResult) {
        case 1:
            strcpy(region, "Old NTSC");
            break;
        case 2:
            strcpy(region, "NTSC");
            break;
        case 3:
            strcpy(region, "PAL");
            break;
        case 4:
            strcpy(region, "PAL-N");
            break;
        default:
            strcpy(region, "Unknown");
    }

    puts(" Done.\nKernal checksum...");
    kernalCRC = get_kernal_crc();
    detect_kernal(kernalCRC, kernal);

    puts(" Done.\nDrives.");
    detect_drive(8, dev8);
    puts("8.");
    detect_drive(9, dev9);
    puts("9.");
    detect_drive(10, dev10);
    puts("10.");
    detect_drive(11, dev11);
    puts("11 Done.\n");

    divLength = 12/*detect_model(regionResult, sidResult, kernalResult, model)*/;
    strcpy(model, "Commodore 64");
    // puts(" Done.");
    clrscr();

    printf("\n \x1F   \xAC\x12\xBE  \xBC\x92\x05      %s\n", model);
    printf(" \x1F  \x12\xBE     \x92\x05      ");
    for (i = 0; i < divLength; i++) {
        putchar('\xC0');
    }
    printf("\n \x1F \x12\xBE      \x92\x05      Kernal: %s\n", kernal);
    printf(" \x1F\xAC\x12   \xA2\x92  \xBC\xA2\xA2\xA2\xA2\xA2\x05 VIC: %s\n", region);
    printf(" \x1F\x12\xBE  \x92\xA1    \x12    \x92\xBE\x05 SID: %s\n", sid);
    puts(" \x1F\x12   \x92     \x12   \x92\xBE\x05  Devices:\n");
    printf(" \x1F\x12   \x92     \x81\xA2\xA2\xA2\x05   %s\n", dev8);
    printf(" \x1F\x12   \x92\xBB    \x81\x12   \xBC\x92\x05  %s\n", dev9);
    printf(" \x1F\x12\xE1  \xBC\x92    \x81\x12    \xBC\x92\x05 %s\n", dev10);
    printf(" \x1F \x12    \x92\xA2\xA2\x12\xBE\x92\x05      %s\n", dev11);
    printf(" \x1F \xBC\x12      \x92\x05      Kernal CRC32: ");
    print_crc32(kernalCRC);
    putchar('\n');
    puts(" \x1F  \xBC\x12\xBB    \x92\n");
    puts(" \x1F    \xBC\x12\xA2\xA2\x92\xBE\n");
    puts("\n \x05Press RETURN to exit.");

    getchar();
    key = 0x00;
    while (key != 0x0D) {
        key = getchar();
    }

    __asm {
        sei
        jmp $fce2
    }
    return 0;
}
