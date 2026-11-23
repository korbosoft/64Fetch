#ifndef DETECT_H
#define DETECT_H

extern unsigned char drive_detected_type[8];
extern unsigned char error_message[32];

#include "drive.h"

unsigned char detect_drive(unsigned char device_number, char *input_string);
unsigned char detect_region(void);
unsigned char detect_sid(void);
// unsigned char detect_cpu(unsigned char sid_detected);
unsigned char detect_kernal(unsigned long crc, char *kernalStr);
unsigned char detect_model(unsigned char ntscpal, unsigned char sid_detected, unsigned char kernal_detected, unsigned char detected_cpu, char *input_string);
unsigned long get_kernal_crc(void);
void init_crc_tables(void);

#endif
