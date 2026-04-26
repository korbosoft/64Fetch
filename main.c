#include <conio.h>
#include <em.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    cputc(nybbleToHexChar((crc >> 28) & 0x0F));
    cputc(nybbleToHexChar((crc >> 24) & 0x0F));
    cputc(nybbleToHexChar((crc >> 20) & 0x0F));
    cputc(nybbleToHexChar((crc >> 16) & 0x0F));
    cputc(nybbleToHexChar((crc >> 12) & 0x0F));
    cputc(nybbleToHexChar((crc >> 8) & 0x0F));
    cputc(nybbleToHexChar((crc >> 4) & 0x0F));
    cputc(nybbleToHexChar(crc & 0x0F));
}

void main() {
    char cpu[10];
    char cpuResult;
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
    POKE(0x289,1);
    POKE(0x28A,0x7F);
    POKE(0x291,0xFF);
    clrscr();
    POKE(0xD020,1);
    POKE(0xD021,0);
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
    cputs(" Done.\r\nRegion...");
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
    cputs(" Done.\r\nKernal checksum...");
    kernalCRC = get_kernal_crc();
    kernalResult = detect_kernal(kernalCRC);
    switch (kernalResult) {
        case 1:
            strcpy(kernal, "901227-01");
            break;
        case 2:
            strcpy(kernal, "901227-02");
            break;
        case 3:
            strcpy(kernal, "901227-03");
            break;
        case 4:
            strcpy(kernal, "251104-04");
            break;
        case 5:
            strcpy(kernal, "251104-01");
            break;
        case 6:
            strcpy(kernal, "901246-01");
            break;
        case 10:
            strcpy(kernal, "JiffyDOS (C64)");
            break;
        case 11:
            strcpy(kernal, "JiffyDOS (SX-64)");
            break;
        default:
            strcpy(kernal, "Unknown");
    }

    cputs(" Done.\r\nCPU...");
    cpuResult = detect_cpu(sidResult);
    switch (cpuResult) {
        case 0:
            strcpy(cpu, "MOS 6502");
            break;
        case 2:
            strcpy(cpu, "WDC 65816");
            break;
        case 9:
            strcpy(cpu, "MOS 8502");
            break;
        case 10:
            strcpy(cpu, "MOS 8500");
            break;
        case 11:
            strcpy(cpu, "MOS 6510");
            break;
        default:
            strcpy(cpu, "Unknown");
    }
    cputs(" Done.\r\nDrives.");
    detect_drive(8, dev8);
    cputs("8.");
    detect_drive(9, dev9);
    cputs("9.");
    detect_drive(10, dev10);
    cputs("10.");
    detect_drive(11, dev11);
    cputs("11 Done.\r\nDetecting Model...");
    divLength = detect_model(regionResult, sidResult, kernalResult, cpuResult, model);
    cputs(" Done.");
    clrscr();
    printf("\n \x1F   \xAC\x12\xBE  \xBC\x92\x05      %s\n", model);
    printf(" \x1F  \x12\xBE     \x92\x05      ");
    for (i = 0; i < divLength; i++) {cputc('\xC0');}
    printf("\n \x1F \x12\xBE      \x92\x05      Kernal: %s", kernal);
    cputs("\r\n");
    printf(" \x1F\xAC\x12   \xA2\x92  \xBC\xA2\xA2\xA2\xA2\xA2\x05 CPU: %s\n", cpu);
    printf(" \x1F\x12\xBE  \x92\xA1    \x12    \x92\xBE\x05 VIC: %s\n", region);
    printf(" \x1F\x12   \x92     \x12   \x92\xBE\x05  SID: %s\n", sid);
    printf(" \x1F\x12   \x92     \x81\xA2\xA2\xA2\x05   %s\n", dev8);
    printf(" \x1F\x12   \x92\xBB    \x81\x12   \xBC\x92\x05  %s\n", dev9);
    printf(" \x1F\x12\xE1  \xBC\x92    \x81\x12    \xBC\x92\x05 %s\n", dev10);
    printf(" \x1F \x12    \x92\xA2\xA2\x12\xBE\x92\x05      %s\n", dev11);
    printf(" \x1F \xBC\x12      \x92\x05      Kernal CRC32: ");
    print_crc32(kernalCRC);
    cputs("\r\n");
    printf(" \x1F  \xBC\x12\xBB    \x92\n");
    printf(" \x1F    \xBC\x12\xA2\xA2\x92\xBE\n");
    printf("\r\n \x05Press RETURN to exit.");
    cgetc();
    key = 0x00;
    while (key != 0x0D) {
        key = cgetc();
    }
    __asm__ ("jsr $FCE2"); // cold reset
}

