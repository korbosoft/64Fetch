/*
 * main.h
 * See 'main.c'.
 */

#define clrstr(str) memset(str, '\0', sizeof(str))

extern unsigned char detect_cpu();
extern unsigned char detect_drive(unsigned char device_number, char * input_string);
extern unsigned char detect_model(unsigned char ntscpal, unsigned char sid_detected, unsigned char kernal_detected, unsigned char detected_cpu, char * input_string);
extern unsigned char detect_region();
extern unsigned char detect_sid();
extern unsigned char detect_speed(unsigned char display_mhz, unsigned char detected_cpu, unsigned char ntscpal, char * input_string);
extern unsigned char detect_kernal();
