/*
- drive.c
- "The drive detection code is a Big One(TM), so it gets it's own file (see 'detect.c')" - Korbo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c64/kernalio.h>
#include "drive.h"

#define clrstr(str) memset(str, '\0', sizeof(str))

unsigned char drive_detected[8];
unsigned char drive_detected_type[8]; // 0 = no drive detected, all others related to a specific drive
unsigned int error_number;
unsigned char error_message[32];
unsigned char error_message2[32];
unsigned int error_track;
unsigned int error_block;
unsigned char result;
unsigned char dev;
signed int read_bytes;
unsigned char disk_buffer[256];

static void upper_string(char *s) {
    while (*s) {
        if (*s >= 'a' && *s <= 'z') {
            *s = (char)(*s - ('a' - 'A'));
        }
        s++;
    }
}

unsigned char string_add_character(unsigned char *input_string, unsigned char character_to_add) {
    unsigned char string_length = (unsigned char)strlen((const char *)input_string);

    switch (string_length) {
        case 63:
            return 0;
        default:
            input_string[string_length] = character_to_add;
            input_string[string_length + 1] = 0;
            return 1;
    }
}

unsigned char open_file_safely(unsigned char file_number, unsigned char device, unsigned char address, unsigned char *command) {
	bool ok;

	supercpu_disable();
	krnio_setnam((const char *)(command ? command : (unsigned char *)""));
	ok = krnio_open((char)file_number, (char)device, (char)address);
	supercpu_enable();

	return ok ? 0 : 1;
}

int read_file_safely(unsigned char file_number, unsigned char *buffer, unsigned int buffer_length) {
	int cbm_read_bytes;

	supercpu_disable();
	cbm_read_bytes = krnio_read((char)file_number, (char *)buffer, (int)buffer_length);
	supercpu_enable();

	return cbm_read_bytes;
}

void close_file_safely(unsigned char file_number) {
	supercpu_disable();
	krnio_close((char)file_number);
	supercpu_enable();
}

int write_file_safely(unsigned char file_number, unsigned char *buffer, unsigned int size) {
	int cbm_written_bytes;

	supercpu_disable();
	cbm_written_bytes = krnio_write((char)file_number, (const char *)buffer, (int)size);
	supercpu_enable();

	return cbm_written_bytes;
}

void process_status(char *input_string) {
	char *p = input_string;
	char *field_start;
	char *comma;
	unsigned char field = 0;

	error_number = 0;
	error_message[0] = '\0';
	error_track = 0;
	error_block = 0;

	if (!p) {
		return;
	}

	while (field < 4) {
		field_start = p;
		comma = strchr(p, ',');
		if (comma) {
			*comma = '\0';
			p = comma + 1;
		} else {
			p = NULL;
		}

		switch (field) {
			case 0:
				error_number = (unsigned int)atoi(field_start);
				break;
			case 1:
				strncpy((char *)error_message, field_start, sizeof(error_message) - 1);
				error_message[sizeof(error_message) - 1] = '\0';
				break;
			case 2:
				error_track = (unsigned int)atoi(field_start);
				break;
			case 3:
				error_block = (unsigned int)atoi(field_start);
				break;
		}

		field++;
		if (!p) {
			break;
		}
	}
}

unsigned char detect_drive(unsigned char device_number, char *input_string) {
    unsigned char cbm_result = 255;

    clrstr(input_string);
    clrstr(error_message);
    clrstr(error_message2);

    set_drive_detection(device_number, 0);
    set_drive_type(device_number, 0);

    cbm_result = open_file_safely(1, device_number, 15, (unsigned char *)"uj");

    switch (cbm_result) {
        case 0:
            set_drive_detection(device_number, 1);
            break;
        default:
            set_drive_detection(device_number, 2);
            close_file_safely(1);
            return 255;
    }

    // ****************************************
    // DRIVE RESET DELAY - TOTAL: 1.3 seconds.
    // ****************************************
    delay_one_second();
    delay_one_decisecond();
    delay_one_decisecond();
    delay_one_decisecond();

    // ****************************************
    // READ DEVICE STRING
    // ****************************************
    result = open_file_safely(1, device_number, 15, (unsigned char *)"");
    read_bytes = read_file_safely(1, disk_buffer, sizeof(disk_buffer) - 1);
    if (read_bytes >= 0 && read_bytes < (int)sizeof(disk_buffer)) {
        disk_buffer[read_bytes] = '\0';
    } else {
        disk_buffer[0] = '\0';
    }
    close_file_safely(1);

    process_status((char *)disk_buffer);

    if (error_message[13] == '1' &&
        error_message[14] == '5' &&
        error_message[15] == '4' &&
        error_message[16] == '1') {
        set_drive_type(device_number, DRIVE_1541);
    } else if (error_message[0] == 'u' &&
               error_message[1] == 'i' &&
               error_message[2] == 'e' &&
               error_message[3] == 'c') {
        set_drive_type(device_number, DRIVE_UIEC);
        strcpy((char *)error_message2, "SD2IEC v");
        string_add_character(error_message2, error_message[6]);
        string_add_character(error_message2, error_message[7]);
        string_add_character(error_message2, error_message[8]);
        string_add_character(error_message2, error_message[9]);
        string_add_character(error_message2, error_message[10]);
        string_add_character(error_message2, error_message[11]);
        strcpy((char *)error_message, (const char *)error_message2);
    } else if (error_message[0] == 83 &&
               error_message[1] == 68 &&
               error_message[2] == 50 &&
               error_message[3] == 73 &&
               error_message[4] == 69 &&
               error_message[5] == 67) {
        set_drive_type(device_number, DRIVE_UIEC);
        strcpy((char *)error_message2, "SD2IEC v");
        string_add_character(error_message2, error_message[8]);
        string_add_character(error_message2, error_message[9]);
        string_add_character(error_message2, error_message[10]);
        string_add_character(error_message2, error_message[11]);
        string_add_character(error_message2, error_message[12]);
        strcpy((char *)error_message, (const char *)error_message2);
    } else if (error_message[0] == 'v' &&
               error_message[1] == 'i') {
        set_drive_type(device_number, DRIVE_VICEFS);
        strcpy((char *)error_message, "virtual drive emu");
    } else if (error_message[13] == '1' &&
               error_message[14] == '5' &&
               error_message[15] == '7' &&
               error_message[16] == '0') {
        set_drive_type(device_number, DRIVE_1570);
    } else if (error_message[13] == '1' &&
               error_message[14] == '5' &&
               error_message[15] == '7' &&
               error_message[16] == '1') {
        set_drive_type(device_number, DRIVE_1571);
    } else if (error_message[10] == 'c' &&
               error_message[22] == '1' &&
               error_message[23] == '5' &&
               error_message[24] == '8' &&
               error_message[25] == '1') {
        set_drive_type(device_number, DRIVE_1581);
        strncpy((char *)error_message, (const char *)(error_message + 10), 16);
        error_message[16] = '\0';
    } else if (error_message[9] == 'j' &&
               error_message[22] == '1' &&
               error_message[23] == '5' &&
               error_message[24] == '8' &&
               error_message[25] == '1') {
        set_drive_type(device_number, DRIVE_1581);
        strncpy((char *)error_message, (const char *)(error_message + 9), 17);
        error_message[17] = '\0';
    } else if (error_message[11] == 'v' &&
               error_message[12] == '1' &&
               error_message[13] == '.' &&
               error_message[14] == '9' &&
               error_message[15] == '2') {
        set_drive_type(device_number, DRIVE_CMDHD);
    } else if (error_message[11] == 'v' &&
               error_message[12] == '1' &&
               error_message[13] == '.' &&
               error_message[14] == '4' &&
               error_message[15] == '0') {
        set_drive_type(device_number, DRIVE_FD2000);
    } else if (error_message[11] == 'v' &&
               error_message[12] == '2' &&
               error_message[13] == '.' &&
               error_message[14] == '3') {
        set_drive_type(device_number, DRIVE_SD2);
    } else if (error_message[15] == 'i' &&
               error_message[16] == 'd' &&
               error_message[17] == 'e' &&
               error_message[18] == '6' &&
               error_message[19] == '4') {
        set_drive_type(device_number, DRIVE_IDE64);
        strcpy((char *)error_message, (const char *)(error_message + 1));
    } else if (error_message[13] == '2' &&
               error_message[14] == '0' &&
               error_message[15] == '3' &&
               error_message[16] == '1') {
        set_drive_type(device_number, DRIVE_2031);
    } else if (error_message[8] == 'v' &&
               error_message[9] == '2' &&
               error_message[10] == '.' &&
               error_message[11] == '7') {
        set_drive_type(device_number, DRIVE_SFD1001);
    } else if (error_message[13] == '4' &&
               error_message[14] == '0' &&
               error_message[15] == '4' &&
               error_message[16] == '0') {
        set_drive_type(device_number, DRIVE_4040);
    } else {
        strcpy((char *)error_message, "Unknown Device");
    }

    upper_string((char *)error_message);
    error_message[23] = '\0';

    if (result == 2 && error_number == 73 && strlen((const char *)error_message) != 4) {
        sprintf(input_string, "%02i: %s", device_number, error_message);
    } else if (cbm_result == 0 && strlen((const char *)error_message) == 4) {
        set_drive_detection(device_number, 2);
        return 255;
    }

    return cbm_result;
}
