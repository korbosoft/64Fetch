/*
 - * detect.c
 - "shoutouts to chironb on github, who i """"borrowed"""" this code from, because i sure as hell wouldn't have been able to do this wizardry, especially not the drive section (see drive.c)" - Korbo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "detect.h"

#define PEEK(addr) (*(volatile unsigned char *)(addr))

static unsigned char tab0[256];
static unsigned char tab1[256];
static unsigned char tab2[256];
static unsigned char tab3[256];

__zeropage static unsigned char *crc_ptr;
__zeropage unsigned char crc0, crc1, crc2, crc3;
static unsigned char t0, t1, t2, t3;

unsigned char detect_region(void) {
    __asm {
        sei
        ldx #$00
        w0:
        lda $d012
        w1:
        cmp $d012
        beq w1
        bmi w0
        and #$03
        cmp #$03
        bne detection_done    // NTSC

        tay
        count_cycles:
        inx
        lda $d012
        bpl count_cycles
        cpx #$5e            // Midpoint between PAL ($50) and PAL-N ($6C)
        bcc is_pal
        iny                 // PAL-N
        is_pal:
        tya
        detection_done:
        sta accu
        lda #$00
        sta accu + 1
        sta accu + 2
        sta accu + 3
        rts
    }
}

unsigned long get_kernal_crc(void) {
    __asm {
        lda #$ff
        sta crc0
        sta crc1
        sta crc2
        sta crc3

        lda #$00
        sta crc_ptr
        lda #$e0
        sta crc_ptr + 1

        crc_loop:
        ldy #$00
        lda (crc_ptr), y
        jsr upd_crc

        inc crc_ptr
        bne check_end
        inc crc_ptr + 1

        check_end:
        lda crc_ptr + 1
        bne crc_loop

        lda crc0
        eor #$ff
        sta accu

        lda crc1
        eor #$ff
        sta accu + 1

        lda crc2
        eor #$ff
        sta accu + 2

        lda crc3
        eor #$ff
        sta accu + 3
        rts

        upd_crc:
        eor crc0
        tax
        lda crc1
        eor tab0, x
        sta crc0
        lda crc2
        eor tab1, x
        sta crc1
        lda crc3
        eor tab2, x
        sta crc2
        lda accu + 3
        eor tab3, x
        sta crc3
        rts
    }
}

void init_crc_tables(void) {
    __asm {
        ldx #$00
        entry_loop:
        stx t0
        lda #$00
        sta t1
        sta t2
        sta t3

        ldy #$08
        bit_loop:
        lsr t3
        ror t2
        ror t1
        ror t0
        bcc no_xor

        lda t0
        eor #$20
        sta t0
        lda t1
        eor #$83
        sta t1
        lda t2
        eor #$b8
        sta t2
        lda t3
        eor #$ed
        sta t3
        no_xor:
        dey
        bne bit_loop

        lda t0
        sta tab0, x
        lda t1
        sta tab1, x
        lda t2
        sta tab2, x
        lda t3
        sta tab3, x

        inx
        bne entry_loop
        rts
    }
}

unsigned char detect_kernal(unsigned long crc, char *kernalStr) {
    switch (crc) {
        case 0xDCE782FA:
            strcpy(kernalStr, "901227-01");   // 901227-01 - R1 C64 First Kernal
            return 1;
        case 0xA5C687B3:
            strcpy(kernalStr, "901227-02");   // 901227-02 - R2 C64 Early Kernal
            return 2;
        case 0xDBE3E7C7:
            strcpy(kernalStr, "901227-03");   // 901227-03 - R3 C64 Kernal
            return 3;
        case 0x2F79984C:
            strcpy(kernalStr, "JiffyDOS (C64)");  // R3 C64 JiffyDOS
            return 10;
            // case 0x00000000:
            //     strcpy(kernalStr, "251104-01");   // 251104-01 - R4 SX-64 Rare First Kernal??
            //     return 5;
        case 0x2C5965D4:
            strcpy(kernalStr, "251104-04");   // 251104-04 - R4 SX-64 Common Latest Kernal
            return 4;
        case 0x2B5A88F5:
            strcpy(kernalStr, "JiffyDOS (SX-64)");  // R3 SX-64 JiffyDOS
            return 11;
        case 0x789C8CC5:
            strcpy(kernalStr, "901246-01");   // 901246-01 - 4064 Educator 64
            return 6;
        case 0xB0A9C2DA:
            strcpy(kernalStr, "390852-01");   // 390852-01 - C64 Games System
            return 8;
        default:
            strcpy(kernalStr, "Unknown");   // Default Unknown Kernal
            return 0;
    }
};

unsigned char detect_sid(void) {
    unsigned char i = 0;

    unsigned char D41B_before = 0;
    unsigned char D41B_after = 0;

    unsigned char one_D41B_after_value = 0;
    unsigned char one_D41B_after_frequency = 0;
    unsigned char two_D41B_after_value = 0;
    unsigned char two_D41B_after_frequency = 0;
    unsigned char three_D41B_after_value = 0;
    unsigned char three_D41B_after_frequency = 0;

    unsigned char likely_8580 = 0;
    unsigned char likely_6581 = 0;

    D41B_before = PEEK(0xD41B);
    (void)D41B_before;

    for (i = 0; i < 255; ++i) {
        *(volatile unsigned char *)0xD011 = 0x0B;
        *(volatile unsigned char *)0xD412 = 0xFF;
        *(volatile unsigned char *)0xD40E = 0xFF;
        *(volatile unsigned char *)0xD40F = 0xFF;
        *(volatile unsigned char *)0xD412 = 0x20;
        *(volatile unsigned char *)0xD011 = 0x1B;
        D41B_after = PEEK(0xD41B);

        if (D41B_after == one_D41B_after_value) {
            ++one_D41B_after_frequency;
        } else if (D41B_after == two_D41B_after_value) {
            ++two_D41B_after_frequency;
        } else if (D41B_after == three_D41B_after_value) {
            ++three_D41B_after_frequency;
        } else if (one_D41B_after_frequency == 0) {
            one_D41B_after_value = D41B_after;
            ++one_D41B_after_frequency;
        } else if (two_D41B_after_frequency == 0) {
            two_D41B_after_value = D41B_after;
            ++two_D41B_after_frequency;
        } else if (three_D41B_after_frequency == 0) {
            three_D41B_after_value = D41B_after;
            ++three_D41B_after_frequency;
        } else {
            D41B_after = 0;
            one_D41B_after_value = 0;
            one_D41B_after_frequency = 0;
            two_D41B_after_value = 0;
            two_D41B_after_frequency = 0;
            three_D41B_after_value = 0;
            three_D41B_after_frequency = 0;
        }
    }

    if (one_D41B_after_value == 0x08 || two_D41B_after_value == 0x08 || three_D41B_after_value == 0x08) {
        ++likely_8580;
    }
    if (one_D41B_after_value == 0x32 || two_D41B_after_value == 0x32 || three_D41B_after_value == 0x32) {
        ++likely_8580;
    }

    if (one_D41B_after_value == 0x09 || two_D41B_after_value == 0x09 || three_D41B_after_value == 0x09) {
        ++likely_6581;
    }
    if (one_D41B_after_value == 0x34 || two_D41B_after_value == 0x34 || three_D41B_after_value == 0x34) {
        ++likely_6581;
    }

    if (likely_6581 > likely_8580) {
        return 1;
    } else if (likely_8580 > likely_6581) {
        return 2;
    } else {
        return 0;
    }
}

// unsigned char detect_cpu(unsigned char sid_detected) {
//     unsigned char gotten_cpu = getcpu();
//
//     if (gotten_cpu == 0) {
//         if (PEEK(0) == 47 && PEEK(0xD030) != 255) {
//             return 9;   // MOS 8502 (Commodore 128)
//         } else if (PEEK(0) == 47 && sid_detected == 2) {
//             return 10;  // MOS 8500 (Commodore 64C)
//         } else if (PEEK(0) == 47 && sid_detected == 1) {
//             return 11;  // MOS 6510 (Commodore 64)
//         } else {
//             return 0;   // MOS 6502 / fallback
//         }
//     } else {
//         return gotten_cpu;
//     }
// }

// unsigned char detect_model(unsigned char ntscpal, unsigned char sid_detected, unsigned char kernal_detected, unsigned char detected_cpu, char *input_string) {
//     const char *result_c64 = "Commodore 64";
//     const char *result_c64c = "Commodore 64C";
//     const char *result_sx64 = "SX-64";
//     const char *result_edu64 = "Educator 64";
//     const char *result_c128 = "Commodore 128";
//     const char *result_c128d = "Commodore 128D";
//     const char *result_c128dcr = "Commodore 128DCR";
//     const char *result_u64 = "Ultimate 64/C64U";
//     const char *result_unknown = "Unknown";
//
//     unsigned char detected_1571 = 0;
//     (void)ntscpal;
//
//     if (PEEK(0xDF00) == 0xFF && PEEK(0xDF1F) == 0xFF) {
//         strcpy(input_string, result_u64);
//         return 16;
//     } else if (kernal_detected == 4 || kernal_detected == 5 || kernal_detected == 11) {
//         strcpy(input_string, result_sx64);
//         return 5;
//     } else if (kernal_detected == 6) {
//         strcpy(input_string, result_edu64);
//         return 11;
//     } else if (detected_cpu == 9) {
//         detected_1571 = (drive_detected_type[0] == 0x71);
//         if (sid_detected == 1) {
//             if (detected_1571 == 1) {
//                 strcpy(input_string, result_c128d);
//                 return 14;
//             }
//             strcpy(input_string, result_c128);
//             return 13;
//         }
//         strcpy(input_string, result_c128dcr);
//         return 16;
//     } else if (sid_detected == 1) {
//         strcpy(input_string, result_c64);
//         return 12;
//     } else if (sid_detected == 2) {
//         strcpy(input_string, result_c64c);
//         return 13;
//     } else {
//         strcpy(input_string, result_unknown);
//         return 7;
//     }
// }
