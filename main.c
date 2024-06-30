#include <conio.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void main() {
    char cpu[10];
    char cpuResult;
    char dev8[36];
    char dev9[36];
    char dev10[36];
    char dev11[36];
    char divLength;
    char i;
    char kernal[16];
    char kernalResult;
    char key;
    char model[17];
    char prev289 = PEEK(0x289);
    char prev28A = PEEK(0x28A);
    char region[9];
    char regionResult;
    char sid[9];
    char sidResult;
    char speed[10];
    POKE(0x289,1);
    POKE(0x28A,0x7F);
    clrscr();
    bgcolor(0);
    printf("\x05");
    printf("Detecting SID...");
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
    printf(" Done.\nDetecting region...");
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
    printf(" Done.\nDetecting kernal revision...");
    kernalResult = detect_kernal();
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
    printf(" Done.\nDetecting CPU...");
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
    printf("\n \x1F \x12\xBE      \x92\x05      KERNAL: %s\n", kernal);
    printf(" \x1F\xAC\x12   \xA2\x92  \xBC\xA2\xA2\xA2\xA2\xA2\x05 CPU: %s %s\n", cpu, speed);
    printf(" \x1F\x12\xBE  \x92\xA1    \x12    \x92\xBE\x05 VIC: %s\n", region);
    printf(" \x1F\x12   \x92     \x12   \x92\xBE\x05  SID: %s\n", sid);
    printf(" \x1F\x12   \x92     \x81\xA2\xA2\xA2\x05   %s\n", dev8);
    printf(" \x1F\x12   \x92\xBB    \x81\x12   \xBC\x92\x05  %s\n", dev9);
    printf(" \x1F\x12\xE1  \xBC\x92    \x81\x12    \xBC\x92\x05 %s\n", dev10);
    printf(" \x1F \x12    \x92\xA2\xA2\x12\xBE\x92\x05      %s\n", dev11);
    printf(" \x1F \xBC\x12      \x92\n");
    printf(" \x1F  \xBC\x12\xBB    \x92\n");
    printf(" \x1F    \xBC\x12\xA2\xA2\x92\xBE\n");
    printf("\n\x05 Press RETURN to exit.");
    key = cgetc();
    if (key == 0x0D) {
        while(key == 0x0D) {
            key = cgetc();
        }
    }
    while(key != 0x0D) {
        key = cgetc();
    }
    POKE(0x289,prev289);
    POKE(0x28a,prev28A);
}

