/*
 * drive.h
 * See 'device.c'.
 */
#ifndef DRIVE_H
#define DRIVE_H

#define PEEK(addr) (*(volatile unsigned char *)(addr))
#define POKE(addr, val) (*(volatile unsigned char *)(addr) = (unsigned char)(val))

#define set_drive_detection(drive, detected) drive_detected[(drive) - 8] = (detected)
#define get_drive_detection(drive)          drive_detected[(drive) - 8]
#define set_drive_type(drive, type)          drive_detected_type[(drive) - 8] = (type)
#define get_drive_type(drive)               drive_detected_type[(drive) - 8]

#define delay_one_decisecond()          \
do {                                    \
    POKE(0xDC08, 0); /* Start TOD Clock. */ \
    while (PEEK(0xDC08) == 0) {}        \
} while (0)

#define delay_one_second()              \
do {                                    \
    POKE(0xDC08, 0); /* Start TOD Clock. */ \
    POKE(0xDC09, 0); /* Start TOD Clock. */ \
    while (PEEK(0xDC09) == 0) {}        \
} while (0)

#define delay_one_rasterscanline()                      \
do {                                                    \
    unsigned char _last_line = PEEK(0xD012);            \
    while (PEEK(0xD012) == _last_line) {}               \
} while (0)

#define supercpu_enable()  POKE(0xD07B, 255)  // Enable SuperCPU (20 MHz mode)
#define supercpu_disable() POKE(0xD07A, 255)  // Disable SuperCPU (1 MHz mode)

#define matching(testing_string, original_string) (strcmp((original_string), (testing_string)) == 0)

// ********************************************************************************
// DRIVE TYPE DEFINITIONS
// ********************************************************************************

// Commodore IEEE Floppy
#define DRIVE_2031    0x31
#define DRIVE_2040    0x20
#define DRIVE_2041    0x21
#define DRIVE_3040    0x30
#define DRIVE_4031    0x31
#define DRIVE_4040    0x44
#define DRIVE_SFD1001 0x01
#define DRIVE_8050    0x50
#define DRIVE_8060    0x60
#define DRIVE_8061    0x61
#define DRIVE_8250    0x50
#define DRIVE_8280    0x80

// Commodore Serial Floppy
#define DRIVE_1540    0x40
#define DRIVE_1541    0x41
#define DRIVE_1542    0x42
#define DRIVE_1551    0x51 // (SFS481)
#define DRIVE_1563    0x63
#define DRIVE_1570    0x70
#define DRIVE_1571    0x71
#define DRIVE_1572    0x72
#define DRIVE_1581    0x81

// Commodore Hard Drive
#define DRIVE_9060    0x96
#define DRIVE_9090    0x99

// CMD Hard Drives
#define DRIVE_HD_40   0xC4
#define DRIVE_HD_170  0xC7
#define DRIVE_HD_340  0xC3
#define DRIVE_HD_500  0xC5
#define DRIVE_HD_1000 0xC1
#define DRIVE_HD_2000 0xC2
#define DRIVE_CMDHD   0xCD
#define DRIVE_FD2000  DRIVE_CMDHD
#define DRIVE_FD4000  DRIVE_CMDHD

// MSD Floppy Drives
#define DRIVE_SD1     0xD1
#define DRIVE_SD2     0xD2

// Modern SD Card Drives
#define DRIVE_UIEC    0xEC
#define DRIVE_IDE64   0x64
#define DRIVE_PI1541  0x1E
#define DRIVE_VICEFS  0xF5

#endif
