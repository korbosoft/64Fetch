#include <conio.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void main() {
    char cpu[11];
    char cpuResult;
    char dev8[36];
    char dev9[36];
    char dev10[36];
    char dev11[36];
    char divLength;
    char i;
    char kernal[10];
    char kernalResult;
    char key;
    char model[17];
    char prev289 = PEEK(0x289);
    char prev28A = PEEK(0x28A);
    char region[9];
    char regionResult;
    char sid[9];
    char sidResult;
    POKE(0x289,1);
    POKE(0x28A,0x7F);
    clrscr();
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
        case 11:
            strcpy(kernal, "JiffyDOS");
            break;
        default:
            strcpy(kernal, "Unknown");
    }
    printf(" Done.\nDetecting CPU...");
    cpuResult = detect_cpu(sidResult);
    switch (cpuResult) {
        case 0:
            strcpy(cpu, "MOS 6502!?");
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
    printf("\n \x9A   \xAC\x12\xBE  \xBC\x92\x05      %s\n", model);
    printf(" \x9A  \x12\xBE     \x92\x05      ");
    for (i = 0; i < divLength; i++) {printf("\xC0");}
    printf("\n \x9A \x12\xBE      \x92\x05      KERNAL: %s\n", kernal);
    printf(" \x9A\xAC\x12   \xA2\x92  \xBC\xA2\xA2\xA2\xA2\xA2\x05 CPU: %s\n", cpu);
    printf(" \x9A\x12\xBE  \x92\xA1    \x12    \x92\xBE\x05 VIC: %s\n", region);
    printf(" \x9A\x12   \x92     \x12   \x92\xBE\x05  SID: %s\n", sid);
    printf(" \x9A\x12   \x92     \x81\xA2\xA2\xA2\x05   %s\n", dev8);
    printf(" \x9A\x12   \x92\xBB    \x81\x12   \xBC\x92\x05  %s\n", dev9);
    printf(" \x9A\x12\xE1  \xBC\x92    \x81\x12    \xBC\x92\x05 %s\n", dev10);
    printf(" \x9A \x12    \x92\xA2\xA2\x12\xBE\x92\x05      %s\n", dev11);
    printf(" \x9A \xBC\x12      \x92\n");
    printf(" \x9A  \xBC\x12\xBB    \x92\n");
    printf(" \x9A    \xBC\x12\xA2\xA2\x92\xBE\n");
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

