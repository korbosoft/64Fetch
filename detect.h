extern unsigned char drive_detected_type[8];
extern unsigned int error_number;
extern unsigned char error_message[32];
extern unsigned int error_track;
extern unsigned int error_block;
extern unsigned char detect_drive(unsigned char device_number, char * input_string);
