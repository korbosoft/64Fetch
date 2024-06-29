#include <conio.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void main() {
    char cpu[11];
    char kernal[10];
    char key;
    char region[14];
    char sidmodel[5];
    char prev289 = PEEK(0x289);
    char prev28A = PEEK(0x28A);
    POKE(0x289,1);
    POKE(0x28A,0x7F);
    clrscr();
    switch (detect_sid()) {
        case 1:
            strcpy(sidmodel, "MOS 6581");
            break;
        case 2:
            strcpy(sidmodel, "MOS 8580");
            break;
        default:
            strcpy(sidmodel, "????");
    }
    switch (detect_region()) {
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
            strcpy(region, "????");
    }
    switch (detect_kernal()) {
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
    switch (detect_cpu()) {
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
            strcpy(cpu, "????");
    }
    printf("\n \x9A   \xAC\x12\xBE  \xBC\x92\x05      64Fetch\n");
    printf(" \x9A  \x12\xBE     \x92\x05      By Korbo\n");
    printf(" \x9A \x12\xBE      \x92\n");
    printf(" \xAC\x12   \xA2\x92  \xBC\xA2\xA2\xA2\xA2\xA2\x05 SID: %s\n", sidmodel);
    printf(" \x9A\x12\xBE  \x92\xA1    \x12    \x92\xBE\x05 VIC: %s\n", region);
    printf(" \x9A\x12   \x92     \x12   \x92\xBE\x05  KERNAL: %s\n", kernal);
    printf(" \x9A\x12   \x92     \x81\xA2\xA2\xA2\x05   CPU: %s\n", cpu);
    printf(" \x9A\x12   \x92\xBB    \x81\x12   \xBC\x92\n");
    printf(" \x9A\x12\xE1  \xBC\x92    \x81\x12    \xBC\x92\n");
    printf(" \x9A \x12    \x92\xA2\xA2\x12\xBE\x92\n");
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

