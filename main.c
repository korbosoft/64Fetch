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

void printHexByte(char byte) {
    cputc(nybbleToHexChar((byte >> 4) & 0x0F));
    cputc(nybbleToHexChar(byte & 0x0F));
}

void main() {
    char cpu[10];
    char cpuResult;
    char dev8[36];
    char dev9[36];
    char dev10[36];
    char dev11[36];
    char divLength;
    char i;
    char kernalResult;
    char kernalChecksum[3];
    char key;
    char model[17];
    char region[9];
    char regionResult;
    char sid[9];
    char sidResult;
    char speed[10];
    POKE(0x289,1);
    POKE(0x28A,0x7F);
    POKE(0x291,0xFF);
    clrscr();
    POKE(0xD020,1);
    POKE(0xD021,0);
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
    printf(" Done.\nRegion...");
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
    printf(" Done.\nKernal checksum...");
    kernalResult = detect_kernal();
    get_checksum(&kernalChecksum[0], &kernalChecksum[1], &kernalChecksum[2]);
    printf(" Done.\nCPU...");
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
    detect_speed(1, cpuResult, regionResult, speed);
    printf(" Done.\nDetecting Drives");
    clrstr(dev8);
    detect_drive(8, dev8);
    printf(".");
    clrstr(dev9);
    detect_drive(9, dev9);
    printf(".");
    clrstr(dev10);
    detect_drive(10, dev10);
    printf(".");
    clrstr(dev11);
    detect_drive(11, dev11);
    printf(" Done.\nDetecting Model...");
    divLength = detect_model(regionResult, sidResult, kernalResult, cpuResult, model);
    printf(" Done.");
    clrscr();
    printf("\n \x1F   \xAC\x12\xBE  \xBC\x92\x05      %s\n", model);
    printf(" \x1F  \x12\xBE     \x92\x05      ");
    for (i = 0; i < divLength; i++) {printf("\xC0");}
    printf("\n \x1F \x12\xBE      \x92\x05      Kernal: ");
    printHexByte(kernalChecksum[0]);
    printHexByte(kernalChecksum[1]);
    printHexByte(kernalChecksum[2]);
    cputs("\r\n");
    printf(" \x1F\xAC\x12   \xA2\x92  \xBC\xA2\xA2\xA2\xA2\xA2\x05 CPU: %s @%s*\n", cpu, speed);
    printf(" \x1F\x12\xBE  \x92\xA1    \x12    \x92\xBE\x05 VIC: %s\n", region);
    printf(" \x1F\x12   \x92     \x12   \x92\xBE\x05  SID: %s\n", sid);
    printf(" \x1F\x12   \x92     \x81\xA2\xA2\xA2\x05   %s\n", dev8);
    printf(" \x1F\x12   \x92\xBB    \x81\x12   \xBC\x92\x05  %s\n", dev9);
    printf(" \x1F\x12\xE1  \xBC\x92    \x81\x12    \xBC\x92\x05 %s\n", dev10);
    printf(" \x1F \x12    \x92\xA2\xA2\x12\xBE\x92\x05      %s\n", dev11);
    printf(" \x1F \xBC\x12      \x92\n");
    printf(" \x1F  \xBC\x12\xBB    \x92\n");
    printf(" \x1F    \xBC\x12\xA2\xA2\x92\xBE\n");
    printf("\n\x05 *CPU MHz is only an approximation.\n");
    printf("\n Press RETURN to exit.");
    while(key != 0x0D) {
        key = cgetc();
    }
    __asm__ ("jsr $FCE2"); // cold reset
}

