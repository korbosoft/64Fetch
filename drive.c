/*
 * device.c
 * "The drive detection code is a Big One(TM), so it gets it's own file (see 'detect.c')" - Korbo
 */

#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

#include "drive.h"

unsigned char string_add_character ( unsigned char * input_string , unsigned char character_to_add ) {

    unsigned char string_length;
    string_length = strlen(input_string);

    switch (string_length) { 								// check if the string is full

        case 63 :
            return(0); 										// return 0 - string full error code
            break;

        default :
            input_string[string_length] = character_to_add; // put new character at one past(strlen) the end of the string(strlen-1)
            input_string[string_length+1] = 0; 				// this shouldn't ever overflow beause this where we are checking it!
            return(1); 										// return 1 - we successfuly added a character to teh string
            //end default

    };//end switch

};//end func


unsigned char open_file_safely( unsigned char file_number , unsigned char device , unsigned char address , unsigned char * command ) {

    unsigned char cbm_open_result;

    supercpu_disable(); // Disable SuperCPU so the kernal timing isn't screwed up.
    cbm_open_result = cbm_open(file_number, device, address, command);
    supercpu_enable();  // Enable SuperCPU so the kernal timing isn't screwed up.

    return(cbm_open_result);

};//end-func

int read_file_safely( unsigned char file_number , unsigned char *buffer , unsigned int buffer_length) {

    int cbm_read_bytes;

    supercpu_disable(); // Disable SuperCPU so the kernal timing isn't screwed up.
    cbm_read_bytes = cbm_read(file_number, buffer, buffer_length);
    // printf("file_number:%u\n",file_number);
    // printf("buffer_length:%u\n",buffer_length);
    // printf("cbm_read_bytes:%u\n",cbm_read_bytes);
    supercpu_enable();  // Enable SuperCPU so the kernal timing isn't screwed up.

    return(cbm_read_bytes);

};//end-func


void close_file_safely( unsigned char file_number ) {

    supercpu_disable(); // Disable SuperCPU so the kernal timing isn't screwed up.
    cbm_close(file_number);
    supercpu_enable();  // Enable SuperCPU so the kernal timing isn't screwed up.

};//end-func


int write_file_safely( unsigned char file_number , unsigned char * buffer , unsigned int size ) {

    // int __fastcall__ cbm_write (unsigned char lfn, const void* buffer, unsigned int size);
    //
    // Writes up to "size" bytes from "buffer" to a file.
    // Returns the number of actually-written bytes, or -1 in case of an error;
    // _oserror contains an error-code, then (see above table).

    int cbm_written_bytes;

    supercpu_disable(); // Disable SuperCPU so the kernal timing isn't screwed up.
    cbm_written_bytes = cbm_write(file_number, buffer, size); // The 1 means we are only writing one byte.
    supercpu_enable();  // Enable SuperCPU so the kernal timing isn't screwed up.

    return(cbm_written_bytes);

};//end-func


void process_status(char * input_string) {
    error_number  = atoi(strtok(input_string, ",")); 	// Extract the first token.
    strcpy(error_message,strtok(NULL, ",")        );	// Extract the second token.
    error_track   = atoi(strtok(NULL, ",")        );    // Extract the third token.
    error_block   = atoi(strtok(NULL, ",")        );	// Extract the fourth token.
};//end func

unsigned char detect_drive(unsigned char device_number, char * input_string) {

    unsigned char cbm_result = 255; // we need to do this

    unsigned char error_message2[16] ;

    set_drive_detection(device_number,0);
    set_drive_type(device_number,0);

    // cbm_result = cbm_open(1, device_number, 15, "uj"); // reset the drive
    // cbm_close(1);

    cbm_result = open_file_safely(1, device_number, 15, "uj"); // reset the drive

    // printf("D%02u: CR:%u ",device_number, cbm_result);

    // close_file_safely(1);

    switch (cbm_result) {
        case 0 :
            set_drive_detection(device_number,1); // successfully detected --> // NOT detected // 0 == not scanned , 1 == detected , 2 == scanned but NOT detected
            break;

            // case 5 :								  // device not present
        default :
            set_drive_detection(device_number,2);
            // cbm_close(1);
            close_file_safely(1);
            return(255); // returns a code that means we didn't see teh drive
            break;

            // default :
            // 	set_drive_detection(device_number,0); // unknown failure; 0 == not successfully scanned either way
            // 	// close_file_safely(1); // TODO: WHY WASN'T THIS NEEDED BEFORE???
            // 	// return(cbm_result); // TODO: WHY WASN'T THIS NEEDED BEFORE???
            //    break;
    };//end switch

    // // #define DRIVE_RESET_WAIT_TIME 23000
    // #define DRIVE_RESET_WAIT_TIME 25000L // TODO: If this doesn't solve Raj's Pi1541 problem, remember to SWITCH IT BACK FOR SPEED!

    // fuckingtimer = 0; 					// Set my fucking variable to 0.
    // POKE(0xDC08,0); 					// Start the TOD Clock by writing to the tenths of second register.
    // wait(DRIVE_RESET_WAIT_TIME); 		// Run my current waiting routine.
    // fuckingtimer = PEEK(0xDC08); 		// Record the value as quickly as possible.
    // printf("ft:%u\n",fuckingtimer); 	// Print the recorded fucking value.

    // TODO: I think this is where Leif was having a problem with his Ultimate whatever freezeing when running in 48MHz mode or whatever baloney it was doing.
    // wait(DRIVE_RESET_WAIT_TIME); // we need to wait a second for teh drive to reset itself. If we don't do this, the computer freezes up when we send the drive a command too soon.
    // TODO: I also think this is where Raj was having a problem. I think the Pi1541 wasn't getting enough time to do a reset.
    // It sucks, but maybe I need to wait a full second? That's probably too much, but for now let's see if it solves the problem!

    // sleep(1); // This should wait 1 second. I hope cc65 is using Jiffies or something.
    // Balls. This just counts cycles dammit.
    // https://github.com/cc65/cc65/blob/master/libsrc/common/sleep.c
    // wait(25000L);

    // ****************************************
    // DRIVE RESET DELAY - TOTAL: 1.3 seconds.
    // ****************************************
    delay_one_second();     // This is the minimum works in VICE.
    delay_one_decisecond(); // I think that the Pi1541,
    delay_one_decisecond(); // and the Ultimate 64,
    delay_one_decisecond(); // both need two more deciseconds.

    // ****************************************
    // READ DEVICE STRING
    // ****************************************

    // result = cbm_open(1, device_number, 15, "");
    // read_bytes = cbm_read(1, disk_buffer, sizeof(disk_buffer));
    // cbm_close(1);

    result = open_file_safely(1, device_number, 15, "");
    read_bytes = read_file_safely(1, disk_buffer, sizeof(disk_buffer));
    close_file_safely(1);

    // printf("sizeof(disk_buffer):%u ",sizeof(disk_buffer));
    // printf("cbm_result:%u ",cbm_result);
    // printf("result:%u ",result);
    // printf("read_bytes:%u ",read_bytes);
    // printf("disk_buffer:%s\n",disk_buffer);

    process_status(disk_buffer);

	// printf("error_message:%s\n",error_message);

	// ****************************************
	// COMMODORE 1541 - IEC
	// ****************************************
	// if        ( matching( "cbm dos v2.6 1541", error_message) ) {
	if ( (error_message[13]=='1' && \
	  	  error_message[14]=='5' && \
		  error_message[15]=='4' && \
		  error_message[16]=='1' ) ){

		set_drive_type(device_number, DRIVE_1541);

	// ****************************************
	// COMMODORE 1541 - JiffyDOS - IEC
	// ****************************************
	// } else if ( matching( "jiffydos 5.0 1541", error_message) ) {
	 	// set_drive_type(device_number, DRIVE_1541);

	// ****************************************
	// SD2IEC OLD FIRMWARE - IEC
	// ****************************************
	// We want to detec the beginning "uiec" so we know it's an SD2IEC, and maybe display "SD2IEC v0.11.3"
	// } else if ( matching( "uiec v0.11.3"      , error_message) ) {
	// Looks like "uiec v0.10.3" -->	v0.10.3 	sd2iec release version 0.10.3 	was the firmware that came with my SD2IEC.
	// Start displaying at position 6
	} else if ( (error_message[0]=='u' && \
 				 error_message[1]=='i' && \
 				 error_message[2]=='e' && \
 				 error_message[3]=='c' ) ){

		set_drive_type(device_number, DRIVE_UIEC );
		strcpy(error_message2,"SD2IEC v");
		string_add_character(error_message2,error_message[ 6]); // TODO: output from position 6 until the end.
		string_add_character(error_message2,error_message[ 7]); // TODO: This is the only place it's used.
		string_add_character(error_message2,error_message[ 8]); // TODO: There's got to be a way to drop this var.
		string_add_character(error_message2,error_message[ 9]);
		string_add_character(error_message2,error_message[10]);
		string_add_character(error_message2,error_message[11]);
		strcpy(error_message,error_message2); // Final string

	// ****************************************
	// SD2IEC NEW FIRMWARE - IEC
	// ****************************************
	// We want to detec the middle "ATENTDEAD" and only display something like: "SD2IEC v1.0.0 24"
	// Use position 8 to 12 (inclusive)
	// Use position 24, 25
	// sd2iec
	} else if ( error_message[0] == 83  && \
 				error_message[1] == 68  && \
 				error_message[2] == 50  && \
 				error_message[3] == 73  && \
 				error_message[4] == 69  && \
 				error_message[5] == 67    ){

		set_drive_type(device_number, DRIVE_UIEC);
		strcpy(error_message2,"SD2IEC v");
		string_add_character(error_message2,error_message[ 8]); // TODO: Dude... like... strncpy error_message+8
		string_add_character(error_message2,error_message[ 9]);
		string_add_character(error_message2,error_message[10]);
		string_add_character(error_message2,error_message[11]);
		string_add_character(error_message2,error_message[12]);

		strcpy(error_message,error_message2); // Final string

	// ****************************************
	// VIRTUAL DRIVE EMULATION / VICE FS DRIVER 2.0 - IEC
	// ****************************************
	// } else if ( matching( "virtual drive emulation", error_message) ) {
	} else if ( (error_message[ 0]=='v' && \
			 	 error_message[ 1]=='i' ) ){
		set_drive_type(device_number, DRIVE_VICEFS);
		strcpy(error_message, "virtual drive emu\0\0\0\0\0\0");

	// // ****************************************
	// // VICE FS DRIVER 2.0 - IEC
	// // ****************************************
	// // } else if ( matching( "vice fs driver v2.0", error_message) ) {
	// } else if ( (error_message[ 0]=='v' && \
	// 		 	    error_message[ 1]=='i' && \
	// 		 	    error_message[ 2]=='c' && \
	// 		 	    error_message[ 3]=='e' ) ){
	// 	set_drive_type(device_number, DRIVE_VICEFS);

	// ****************************************
	// COMMODORE 1570 - IEC
	// ****************************************
	// } else if ( matching( "cbm dos v3.0 1570", error_message) ) {
	} else if ( (error_message[13]=='1' && \
			 	 error_message[14]=='5' && \
			 	 error_message[15]=='7' && \
			 	 error_message[16]=='0' ) ){
		set_drive_type(device_number, DRIVE_1570);

	// ****************************************
	// COMMODORE 1570 - JiffyDOS - IEC
	// ****************************************
	// I can't find this. I don't think one was made??? Maybe it uses the 1541? There's a modded version out there on a German site.
	// } else if ( matching( "jiffydos 6.0 1571?!?!?", error_message) ) {
	// 	set_drive_type(device_number, DRIVE_1571);

	// ****************************************
	// COMMODORE 1571 - IEC
	// ****************************************
	// } else if ( matching( "cbm dos v3.0 1571", error_message) ) {
	} else if ( (error_message[13]=='1' && \
			 	 error_message[14]=='5' && \
			 	 error_message[15]=='7' && \
			 	 error_message[16]=='1' ) ){
		set_drive_type(device_number, DRIVE_1571);

	// ****************************************
	// COMMODORE 1571 - JiffyDOS - IEC
	// ****************************************
	// } else if ( matching( "jiffydos 6.0 1571", error_message) ) {
	// 	set_drive_type(device_number, DRIVE_1571);


	// ****************************************
	// COMMODORE 1581 - IEC
	// ****************************************
	// } else if ( matching( "copyright cbm dos v10 1581", error_message) ) {
	} else if ( (error_message[10]=='c' && \
				 error_message[22]=='1' && \
			 	 error_message[23]=='5' && \
			 	 error_message[24]=='8' && \
			 	 error_message[25]=='1' ) ){
		set_drive_type(device_number, DRIVE_1581);
		strncpy(error_message,error_message+10,16);
		error_message[16] = '\0';

	// ****************************************
	// COMMODORE 1581 - JIFFYDOS - IEC
	// ****************************************
	// } else if ( matching( "(c) 1989 jiffydos 6.0 1581", error_message) ) {
	} else if ( (error_message[ 9]=='j' && \
				 error_message[22]=='1' && \
			 	 error_message[23]=='5' && \
			 	 error_message[24]=='8' && \
			 	 error_message[25]=='1' ) ){
		set_drive_type(device_number, DRIVE_1581);
		strncpy(error_message,error_message+9,17);
		error_message[17] = '\0';  // "JiffyDOS 6.0 1581"

	// ****************************************
	// CMD HD - IEC
	// ****************************************
	// } else if ( matching( "cmd hd dos v1.92", error_message) ) {
	} else if ( (error_message[11]=='v' && \
			 	 error_message[12]=='1' && \
			 	 error_message[13]=='.' && \
			 	 error_message[14]=='9' && \
			 	 error_message[15]=='2' ) ){
		set_drive_type(device_number, DRIVE_CMDHD);

	// ****************************************
	// COMMODORE CMD FD-2000 / FD-4000 - IEC
	// ****************************************
	// } else if ( matching( "cmd fd dos v1.40", error_message) ) {
	} else if ( (error_message[11]=='v' && \
			 	 error_message[12]=='1' && \
			 	 error_message[13]=='.' && \
			 	 error_message[14]=='4' && \
			 	 error_message[15]=='0' ) ){
		set_drive_type(device_number, DRIVE_FD2000); 				// Note: This is currently just using DRIVE_CMDHD because all these drives should work in exactly the same way.

	// ****************************************
	// MSD SD-2 - IEC *OR* IEEE-488
	// ****************************************
	// } else if ( matching( "m.s.d. dos v2.3", error_message) ) {
	} else if ( (error_message[11]=='v' && \
			 	 error_message[12]=='2' && \
			 	 error_message[13]=='.' && \
			 	 error_message[14]=='3' ) ){
		set_drive_type(device_number, DRIVE_SD2);

	// ****************************************
	// MSD SD-2 - JiffyDOS - IEC *OR* IEEE-488
	// ****************************************
	// TODO: Maybe I should order a ROM from Jim for this drive.

	// ****************************************
	// IDE64 - Cartridge
	// ****************************************
	// } else if ( matching( " ide dos v0.90 ide64", error_message) ) {
	} else if ( (error_message[15]=='i' && \
			 	 error_message[16]=='d' && \
			 	 error_message[17]=='e' && \
			 	 error_message[18]=='6' && \
			 	 error_message[19]=='4' ) ){
		set_drive_type(device_number, DRIVE_IDE64);
		// strcpy(error_message,"ide dos v0.90 ide64");
		strcpy(error_message,error_message+1);

	// ****************************************
	// COMMODORE 2031 - IEEE-488
	// ****************************************
	// } else if ( matching( "cbm dos v2.6 2031", error_message) ) {
	} else if ( (error_message[13]=='2' && \
			 	 error_message[14]=='0' && \
			 	 error_message[15]=='3' && \
			 	 error_message[16]=='1' ) ){
		set_drive_type(device_number, DRIVE_2031);

	// ****************************************
	// COMMODORE SFD-1001 - IEEE-488
	// ****************************************
	// } else if ( matching( "cbm dos v2.7", error_message) ) { 	// TODO: Add SFD-1001 to the end of the string like the 2031.
	} else if ( (error_message[ 8]=='v' && \
			 	 error_message[ 9]=='2' && \
			 	 error_message[10]=='.' && \
			 	 error_message[11]=='7' ) ){
		set_drive_type(device_number, DRIVE_SFD1001);

	// ****************************************
	// COMMODORE 4040 - IEEE-488
	// ****************************************
	} else if ( matching( "cbm dos v2", error_message) ) { 		// TODO: Add 4040 to the end of the string like the 2031.
		set_drive_type(device_number, DRIVE_4040);


	// ****************************************
	// Pi1541
	// ****************************************
	// } else if ( matching( "pi1541 v00.00", error_message) ) { // This isn't what I should use. It's just a reminder. I'll search just for the beginning part.
	//	set_drive_type(device_number, DRIVE_PI1541);

	// https://github.com/pi1541/Pi1541/blob/master/src/iec_commands.cpp
	// Starting Line: 165
	// case ERROR_73_DOSVERSION:
	// 	snprintf(ErrorMessage, sizeof(ErrorMessage)-1, "%02d,PI1541 V%02d.%02d,%02d,%02d\r", errorCode,
	// 				versionMajor, versionMinor, track, sector);

	// ****************************************
	// UNKNOWN DRIVE
	// ****************************************
	} else {
		// Do nothing. // TODO: TODO: TODO: ***** Maybe create an actual DRIVE_UNKNOWN. so I can output a "IEC Device" string *IF* the device itself *DOES NOT* send anything. Like The Flyer DAMMIT! This would help with debugging.
		strcpy(error_message, "Unknown Device\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
	};//end if


	// TODO: It looks like it's super easy to read about 4 characters at the end of string to identify them.
	// TODO: I can also do this because the smart JiffyDOS makers made the strings in the same length and format.
	// TODO: This means it's easy to read both:
	// TODO: "cbm dos v2.6 1541" and
	// TODO: "jiffydos 5.0 1541" because they line up. It saves an entire check!
	// TODO: This would also save *a lot* of space in the program,
	// TODO: because I don't have to store and check against the entire string!!!
	// TODO: This is way better than trying to use Pearson Hashing!!!!!
	// TODO: I was originally going to use this --> https://en.wikipedia.org/wiki/Pearson_hashing
	// TODO: That way I could check against the entire string without storing all the strings to check.
	// TODO: However, I would have needed to also have a block 0 through 255 in a scambled array,
	// TODO: which was going to take up more room than it was worth.
	// TODO: Although I was going to use my basic tokens array as this anyway,
	// TODO: even though it wasn't totally correct.
	// TODO: This is worth exploring anyway, perhaps in a stand alone program just to see how it works.
	// TODO: If this does work, I could use it to store a list of commands and built-in aliases, saving a bunch!
	// TODO: This is worth exploring!

	strupper(error_message);

    // TODO: This should go into a section below, so we only cut it off at length 22 when it's being displayed as part of the sysinfo or title screen.
    error_message[23]='\0'; // GitHub Issue#???: Text too makes it look bad: SD2IEC V1.0.0ATENTDEAD0-24 - Testing: // strcpy(error_message,"SD2IEC V1.0.0ATENTDEAD0-24");

    // NEW! Mon Dec 13 505PM
    // This is to try and address the weird display of null strings with that stupid /6_O string.
    // TODO: This is a kludgy fix. The /6_O string.it whatever I see when there's some weird unuised or unformatted or wahtever string fuck I hate this. It matches some text starting rigth at 0x0000 so WTF??? it's like a null pointer or somethign but it's not. Fuck fuck fuck.
    if ( result == 2 && error_number == 73 && strlen(error_message) != 4 ) {
        sprintf(input_string, "%02i: %s", device_number, error_message ); //  everything is okay, and we go the drive identity
    } else if ( cbm_result == 0 && strlen(error_message) == 4 ) {
        set_drive_detection(device_number,2);  // Set device to not present, because this is the special BusCardII situation.
        return(255); // This means that it detected a drive, but the drive returned no string. This is a workaround for the BusCardII IEEE-488 Commodore 64 cartridge. This device returns a result as if a drive was detected, but there's no attached drive, and so there's no drive string returned. Which leaves a bunch of blank lines which we are trying not to do here.
    };//end if

	return(cbm_result);

};//end func
