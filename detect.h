extern unsigned char drive_detected_type[8];
extern unsigned char error_message[32];
extern unsigned char detect_cpu(unsigned char sid_detected);
extern unsigned char detect_drive(unsigned char device_number, char * input_string);
extern unsigned char detect_model(unsigned char ntscpal, unsigned char sid_detected, unsigned char kernal_detected, unsigned char detected_cpu, char * input_string);
extern unsigned char detect_region();
extern unsigned char detect_sid();
extern void detect_speed(unsigned char display_mhz, unsigned char detected_cpu, unsigned char ntscpal, char * input_string);
extern unsigned char detect_kernal();
extern unsigned long __fastcall__ get_kernal_crc(void);
extern void __fastcall__ init_crc_tables(void);

