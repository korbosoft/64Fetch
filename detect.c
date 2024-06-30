/*
 * detect.c
 * "shoutouts to chironb on github, who i """"borrowed"""" this code from, because i sure as hell wouldn't have been able to do this wizardry, especially not the drive section (see drive.c)" - Korbo
 */

#include <6502.h>
#include <peekpoke.h>
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

    unsigned char i 						 = 0; // For looping

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

};//end func

unsigned char detect_cpu(void) {

    unsigned char gotten_cpu   = 0;
    unsigned char sid_detected = 0;

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
    sid_detected = detect_sid();

    if ( getcpu() == 0 ) {
        if        (PEEK(0) == 47 && PEEK(0xD030) != 255) { return(9 ); // 9  MOS 8502 (Commodore 128)
        } else if (PEEK(0) == 47 && sid_detected == 2  ) { return(10); // 10 MOS 8500 (Commodore 64C)
        } else if (PEEK(0) == 47 && sid_detected == 1  ) { return(11); // 11 MOS 6510 (Commodore 64)
        } else 											 { return(0);  // 0  MOS 6502 ???
        };
    } else 												 { return(gotten_cpu); // 255 Unknown
    };

};//end func

unsigned char detect_kernal(void) {
    // I have discovered a *NEW* byte to look at in any kernal and figure out what it is. I need to re-write this to use this, becuase it'll save a bunch of code!

    // 	REVIEW - E4AC (58540) - Each Kernal:
    // - 901227-01 --> 0x2B
    // - 901227-02 --> 0x5C
    // - 901227-03 --> 0x81
    // - 251104-01 --> 0x00
    // - 251104-04 --> 0xB3
    // - 901246-01 --> 0x63

    // C64 R3 58497 _77_ && 58498 79 && 58677 14
    // SX  R3 58497 _83_ && 58498 88 && 58677 6
    // J64 R3 58497 _68_ && 58498 79 && 58677 14
    // JSX R3 58497 _68_ && 58498 79 && 58677 6

    unsigned char kernal_value = 0;
    kernal_value = PEEK(0xE4AC);

    switch (kernal_value) {
        case 0x2B : return(1); break;					// 901227-01 - R1 C64 First Kernal
        case 0x5C : return(2); break;					// 901227-02 - R2 C64 Early Kernal
        case 0x81 :
            switch ( PEEK(58497L) ) {
                case 68 :
                    switch (PEEK(58677L)) {
                        case 14 : return(10); break;	// R3 C64 JiffyDOS
                        case  6 : return(11); break;	// R3 SX-64 JiffyDOS
                    };//end-switch
                    break;
            };//end-switch
            return(3); 									// 901227-03 - R3 C64 Kernal
            break;
                        case 0xB3 : return(4); break;					// 251104-04 - R4 SX-64 Common Latest Kernal
                        case 0x00 :	return(5); break;					// 251104-01 - R1 SX-64 Rare First Kernal
                        case 0x63 : return(6); break;					// 901246-01 - 4064 Educator 64 --> ? Fair to say this is most similar to a stock R3 901227-03 kernal. From here: "Note that some patches of 901227-03 are included." http://www.zimmers.net/anonftp/pub/cbm/firmware/computers/c64/revisions.txt
                        default   : return(0); break;  					// Default Unknown Kernal
    };//end switch

}; // end func

unsigned char detect_model(unsigned char ntscpal, unsigned char sid_detected, unsigned char kernal_detected, unsigned char detected_cpu, char * input_string) {

    unsigned char ntscpal_detected = 0;
    unsigned char detected_1571    = 0;

    unsigned char result = 0;
    // TODO: !!!!!!!!!!!!!!! DELETE THE LINE BELOW! IT'S NOT USED AND IS STUPID!!!
    // unsigned char disk_buffer[40] = " "; // TOODO: Look up how big this string can actually get in Commodore DOS / 1541 stuff...
    unsigned char read_bytes = 0;

    // First, why am I doing this twice? This whole function is a waste and needs to be tighten up a lot
    ntscpal_detected = ntscpal <= 2 ;;

    if (detected_cpu == 9) {
        detect_drive(8, 0);
        detected_1571=(drive_detected_type[0]==0x71); // If we've got a 1571 on device 8 then maybe we've got a C128D
    };//end-if

    // SID Return Values:		// CPU Return Values:
    // 1 - 6581 				// 9  - MOS 8502 Commodore 128
    // 2 - 8580 				// 10 - MOS 8500 Commodore 64C
    // 0 - Undetermined 		// 11 - MOS 6510 Commodore 64

    if        (kernal_detected  == 4 || kernal_detected == 5 || kernal_detected == 11) { strcpy(input_string, "SX-64"); return(5);   // SX-64 251104-01 or 251104-04 Kernal ROM
    } else if (kernal_detected  == 6 )                                                 { strcpy(input_string, "Educator 64"); return(11);   // Educator 64
        // TODO: THIS CANNOT BE 0 SINCE THERE"S NO ELS CONDITION!!!!! Need to rework! it's a SID problem!
    } else if (detected_cpu     == 9 &&    sid_detected == 1 &&   detected_1571 == 1)  { strcpy(input_string, "Commodore 128D"); return(14);   // 128D       CPU:8502 GPU:8564 NTSC or 8566/69 PAL SID:6581 Drive: Must have 1571
    } else if (detected_cpu     == 9 &&    sid_detected == 1 )                         { strcpy(input_string, "Commodore 128"); return(13);   // 128        CPU:8502 GPU:8564 NTSC or 8566/69 PAL SID:6581 Drive: Anything other than 1571
    } else if (detected_cpu     == 9 &&    sid_detected == 2 )     					   { strcpy(input_string, "Commodore 128DCR"); return(16);   // 128DCR     CPU:8502 GPU:8564 NTSC or 8566/69 PAL SID:8580 Don't need to worry about the drive, because the DCR is the only machine with the 8502 CPU and 8580 SID.
    } else if (ntscpal_detected == 0 &&    sid_detected == 1 && kernal_detected == 1)  { strcpy(input_string, "Commodore 64"); return(12);   // 64 (Early) NTSC + 6581 + KERNAL R1 901227-01 --> C64 (Early) --> VIC-II 6567
    } else if (ntscpal_detected == 0 &&    sid_detected == 1 )                         { strcpy(input_string, "Commodore 64"); return(12);   // 64         NTSC + 6581 + KERNAL R2 901227-02 --> C64 	      --> VIC-II 6567
    } else if (ntscpal_detected == 0 &&    sid_detected == 2 )                         { strcpy(input_string, "Commodore 64C"); return(13);   // 64C        NTSC + 8580 + Any		 	      --> C64C 	      --> VIC-II 8562
    } else if (ntscpal_detected == 1 &&    sid_detected == 1 )                         { strcpy(input_string, "Commodore 64"); return(12);   // 64         PAL  + 6581 + Any 			      --> C64 		  --> VIC-II 6569/6572/6573
    } else if (ntscpal_detected == 1 &&    sid_detected == 2 )                         { strcpy(input_string, "Commodore 64C"); return(13);   // 64C        PAL  + 8580 + Any		  		  --> C64C 	      --> VIC-II 8565
    } else 																			   { strcpy(input_string, "Unknown"); return(7); // Couldn't detect a specific model!
    };//end if

};//end func
