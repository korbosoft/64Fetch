#include <conio.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "detect.h"
void main() {
    char sidmodel[5];
    char region[14];
    char key;
    char revision[11];
    POKE(0x289,1);
    POKE(0x28a,127);
    clrscr();
    switch (detectsid()) {
        case 2:
            strcpy(sidmodel, "6581");
            break;
        case 3:
            strcpy(sidmodel, "8580");
            break;
        default:
            strcpy(sidmodel, "????");
    }
    switch (detectregion()) {
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
    switch (PEEK(65408L)) {
        case 170:
            strcpy(revision, "Revision 1");
            break;
        case 0:
            strcpy(revision, "Revision 2");
            break;
        case 3:
            strcpy(revision, "Revision 3");
            break;
        case 67:
            strcpy(revision, "SX-64");
            break;
        case 100:
            strcpy(revision, "4064");
            break;
        default:
            strcpy(revision, "Unknown");
    }
    printf("\n \x9A   \xAC\x12\xBE  \xBC\x92\x05      64Fetch\n");
    printf(" \x9A  \x12\xBE     \x92\x05      By Korbo\n");
    printf(" \x9A \x12\xBE      \x92\n");
    printf(" \xAC\x12   \xA2\x92  \xBC\xA2\xA2\xA2\xA2\xA2\x05 SID: %s\n", sidmodel);
    printf(" \x9A\x12\xBE  \x92\xA1    \x12    \x92\xBE\x05 VIC: %s\n", region);
    printf(" \x9A\x12   \x92     \x12   \x92\xBE\x05  KERNAL: %s\n", revision);
    printf(" \x9A\x12   \x92     \x81\xA2\xA2\xA2\n");
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
    POKE(0x289,10);
    POKE(0x28a,0);
}

