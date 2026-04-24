/*
 * detect.c
 * "shoutouts to chironb on github, who i """"borrowed"""" this code from, because i sure as hell wouldn't have been able to do this wizardry, especially not the drive section (see drive.c)" - Korbo
 */

#include <6502.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "detect.h"

unsigned char detect_sid(void) {

    // **********
    // DETECT SID
    // **********

    // Taken from the work published here:

    // https://codebase64.org/doku.php?id=base:detecting_sid_type_-_safe_method

    // ;SID DETECTION ROUTINE

    // 	;By SounDemon - Based on a tip from Dag Lem.
    // 	;Put together by FTC after SounDemons instructions
    // 	;...and tested by Rambones and Jeff.

    // 	; - Don't run this routine on a badline
    // 	; - Won't work in VICE (always detects 6581) unless resid-fp emulation is enabled

    // 	sei		    ;No disturbing interrupts
    // 	lda #$ff
    // 	cmp $d012	;Don't run it on a badline.
    // 	bne *-3

    // 	;Detection itself starts here
    // 	lda #$ff	;Set frequency in voice 3 to $ffff
    // 	sta $d412	;...and set testbit (other bits doesn't matter) in $d012 to disable oscillator
    // 	sta $d40e
    // 	sta $d40f
    // 	lda #$20	;Sawtooth wave and gatebit OFF to start oscillator again.
    // 	sta $d412
    // 	lda $d41b	;Accu now has different value depending on sid model (6581=3/8580=2)
    // 	lsr		;...that is: Carry flag is set for 6581, and clear for 8580.
    // 	bcc model_8580
    // model_6581:
    // 	[...]

    // model_8580:
    // 	[...]

    register unsigned char i 				 = 0; // For looping

    unsigned char D41B_before  				 = 0;
    unsigned char D41B_after   				 = 0;

    unsigned char one_D41B_after_value       = 0;
    unsigned char one_D41B_after_frequency   = 0;
    unsigned char two_D41B_after_value       = 0;
    unsigned char two_D41B_after_frequency   = 0;
    unsigned char three_D41B_after_value     = 0;
    unsigned char three_D41B_after_frequency = 0;

    unsigned char likely_8580 				 = 0;
    unsigned char likely_6581 				 = 0;

    D41B_before = PEEK(0xD41B); // Record iniital value

    for (i = 0 ; i < 255 ; ++i) {

        POKE(0xD011, 0x0B); 		// Load 0x0B into 0xD011(53265) - Disable VIC-II
        POKE(0xD412, 0xFF); 		// Load 0xFF into 0xD412   		- Set frequency in voice 3 to $ffff
        POKE(0xD40E, 0xFF); 		// Load 0xFF into 0xD40E   		- ...and set testbit (other bits doesn't matter) in $d012 to disable oscillator
        POKE(0xD40F, 0xFF); 		// Load 0xFF into 0xD40F
        POKE(0xD412, 0x20); 		// Load 0x20 into 0xD412        - Sawtooth wave and gatebit OFF to start oscillator again.
        POKE(0xD011, 0x1B); 		// Load 0x1B into 0xD011(53265) - Enable VIC-II
        D41B_after = PEEK(0xD41B);	// Record current value

        // check curent value to see if it's sotred
        if 			( D41B_after == one_D41B_after_value ) { 	// does the current value equal slot one's value
            ++one_D41B_after_frequency;							// if so, update the count for this slot

        } else if 	( D41B_after == two_D41B_after_value ) { 	// does the current value equal slot two's value
            ++two_D41B_after_frequency;							// if so, update the count for this slot

        } else if 	( D41B_after == three_D41B_after_value ) {  // does the current value equal slot three's value
            ++three_D41B_after_frequency;						// if so, update the count for this slot

            // if it doesn't match any slots, try to allocate one
        } else if 	( one_D41B_after_frequency == 0 ) {			// if this slot one empty?
            one_D41B_after_value = D41B_after;					// if so, update the value for this slot and...
            ++one_D41B_after_frequency;							// ...update the count for this slot

        } else if 	( two_D41B_after_frequency == 0 ) {			// if this slot one empty?
            two_D41B_after_value = D41B_after;					// if so, update the value for this slot and...
            ++two_D41B_after_frequency;							// ...update the count for this slot

        } else if 	( three_D41B_after_frequency == 0 ) {		// if this slot one empty?
            three_D41B_after_value = D41B_after;				// if so, update the value for this slot and...
            ++three_D41B_after_frequency;						// ...update the count for this slot

        } else {
            D41B_after   			   = 0;
            one_D41B_after_value       = 0;
            one_D41B_after_frequency   = 0;
            two_D41B_after_value       = 0;
            two_D41B_after_frequency   = 0;
            three_D41B_after_value     = 0;
            three_D41B_after_frequency = 0;

        };

    };

    // Scan for 8580 --> 0x08 & any of 0x33 0x32
    if (one_D41B_after_value == 0x08 || two_D41B_after_value == 0x08 || three_D41B_after_value == 0x08) {
        ++likely_8580;
    };

    if (one_D41B_after_value == 0x32 || two_D41B_after_value == 0x32 || three_D41B_after_value == 0x32) {
        ++likely_8580;
    };

    // Scan for 6581 --> 0x09 & any of 0x34 0x33
    if (one_D41B_after_value == 0x09 || two_D41B_after_value == 0x09 || three_D41B_after_value == 0x09) {
        ++likely_6581;
    };

    if (one_D41B_after_value == 0x34 || two_D41B_after_value == 0x34 || three_D41B_after_value == 0x34) {
        ++likely_6581;
    };

    // Codes:
    // 1 - 6581
    // 2 - 8580
    // 0 - Undetermined
    if 		  (likely_6581 > likely_8580) {
        return(1);
    } else if (likely_8580 > likely_6581) {
        return(2);
    } else {
        return(0);
    };

};

unsigned char detect_cpu(unsigned char sid_detected) {

    unsigned char gotten_cpu  = 0;
    unsigned char peek0 = PEEK(0);

    // cc65 has a getcpu() function that only tells you if it's a 6502 or others, but not the Commodore specific ones.
    // The cc65 online documentation is super old. The GitHub has more up-to-date information.
    // https://github.com/cc65/cc65/blob/master/include/6502.h
    //
    // /* Possible returns of getcpu() */
    // #define CPU_6502        0
    // #define CPU_65C02       1
    // #define CPU_65816       2
    // #define CPU_4510        3
    // #define CPU_65SC02      4
    // #define CPU_65CE02      5
    // #define CPU_HUC6280     6
    // #define CPU_2A0x        7
    // #define CPU_45GS02      8

    // Detect 6510/8500/8502 --> Address 0x00 should be 47, whereas regular 6502's seem to have 76 there.
    // Detect SID to guess that it's a Commodore 64C or not.
    // Detect a Commodore 128 --> Address 0xD030 (53296) is always 255 on a non-Commodore 128 6502 CPU.

    gotten_cpu   = getcpu();

    if ( gotten_cpu == 0 ) {
        if        (PEEK(0) == 47 && PEEK(0xD030) != 255) { return(9 ); // 9  MOS 8502 (Commodore 128)
        } else if (PEEK(0) == 47 && sid_detected == 2  ) { return(10); // 10 MOS 8500 (Commodore 64C)
        } else if (PEEK(0) == 47 && sid_detected == 1  ) { return(11); // 11 MOS 6510 (Commodore 64)
        } else 											 { return(0);  // 0  MOS 6502 ???
        };
    } else 												 { return(gotten_cpu); // 255 Unknown
    };

};

unsigned char detect_kernal(unsigned long crc) {
    switch (crc) {
        case 0xDCE782FA : return(1); break;					// 901227-01 - R1 C64 First Kernal
        case 0xA5C687B3 : return(2); break;					// 901227-02 - R2 C64 Early Kernal
        case 0xDBE3E7C7 : return(3); break; 									// 901227-03 - R3 C64 Kernal
        case 0x2F79984C : return(10); break;	// R3 C64 JiffyDOS
        case 0x2C5965D4 : return(4); break;					// 251104-04 - R4 SX-64 Common Latest Kernal
        // case 0x00000000 :	return(5); break;					// 251104-01 - R1 SX-64 Rare First Kernal
        case 0x2B5A88F5 : return(11); break;	// R3 SX-64 JiffyDOS
        case 0x789C8CC5 : return(6); break;					// 901246-01 - 4064 Educator 64
        case 0xB0A9C2DA : return(8); break;					// 390852-01 - C64 Games System
        default   : return(0); break;  					// Default Unknown Kernal
    };

};

unsigned char detect_model(unsigned char ntscpal, unsigned char sid_detected, unsigned char kernal_detected, unsigned char detected_cpu, char * input_string) {
    const char* result_c64 = "Commodore 64";
    const char* result_c64c = "Commodore 64C";
    const char* result_sx64 = "SX-64";
    const char* result_edu64 = "Educator 64";
    const char* result_c128 = "Commodore 128";
    const char* result_c128d = "Commodore 128D";
    const char* result_c128dcr = "Commodore 128DCR";
    const char* result_u64 = "Ultimate 64/C64U";
    const char* result_unknown = "Unknown";

    unsigned char ntscpal_detected = 0;
    unsigned char detected_1571    = 0;

    unsigned char result = 0;

    ntscpal_detected = ntscpal <= 2;

    if (detected_cpu == 9) {

    };

    // SID Return Values:		// CPU Return Values:
    // 1 - 6581 				// 9  - MOS 8502 Commodore 128
    // 2 - 8580 				// 10 - MOS 8500 Commodore 64C
    // 0 - Undetermined 		// 11 - MOS 6510 Commodore 64
    if (PEEK(0xDF00) == 0xFF && PEEK(0xDF1F) == 0xFF) {
        strcpy(input_string, result_u64);
    } else if (kernal_detected == 4 || kernal_detected == 5 || kernal_detected == 11) {
        // SX-64 251104-01 or 251104-04 Kernal ROM
        strcpy(input_string, result_sx64); return(5);
    } else if (kernal_detected == 6) {
        // Educator 64
        strcpy(input_string, result_edu64); return(11);
    } else if (detected_cpu == 9) {
        // detect_drive(8, 0);
        detected_1571=(drive_detected_type[0]==0x71); // If we've got a 1571 on device 8 then maybe we've got a C128D
        if (sid_detected == 1) {
            if (detected_1571 == 1) {
                // 128D CPU:8502 GPU:8564 NTSC or 8566/69 PAL SID:6581 Drive: Must have 1571
                strcpy(input_string, result_c128d); return(14);
            }
            // 128 CPU:8502 GPU:8564 NTSC or 8566/69 PAL SID:6581 Drive: Anything other than 1571
            strcpy(input_string, result_c128); return(13);
        }
        // 128DCR CPU:8502 GPU:8564 NTSC or 8566/69 PAL SID:8580
        // Don't need to worry about the drive, because the DCR is the only machine with the 8502 CPU and 8580 SID.
        strcpy(input_string, result_c128dcr); return(16);
    } else if (sid_detected == 1) {
        // 64 (Early) NTSC + 6581 + KERNAL R1 901227-01 --> C64 (Early) --> VIC-II 6567
        strcpy(input_string, result_c64); return(12);
    } else if (sid_detected == 2) {
        // 64C NTSC + 8580 + Any --> C64C --> VIC-II 8562
        strcpy(input_string, result_c64c); return(13);
    } else {
        // Couldn't detect a specific model!
        strcpy(input_string, result_unknown); return(7);
    };

};
