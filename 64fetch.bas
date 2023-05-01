CHARSET 3:MEMSET 1024, 1000, 32:BACKGROUND 0:BORDER 0:POKE 646,1:LOCATE 0,0:PRINT "64Fetch{RETURN}"
DIM FAST TEMP AS BYTE
PRINT "VIC-II: ";
ASM
    SEI
    LDX #$00
w0  LDA $D012
w1  CMP $D012
    BEQ w1
    BMI w0
    AND #$03
    CMP #$03
    BNE detectionDone ; done for NTSC
    TAY
countCycles
    INX
    LDA $D012
    BPL countCycles
    CPX #$5E  ; VICE values: PAL-N=$6C PAL=$50
	      ; so choose middle value $5E for check
    BCC isPAL
    INY ; is PAL-N
isPAL
    TYA
detectionDone
    STA {TEMP}
END ASM
ON TEMP - 1 GOTO ONTSC, NTSC, PAL, PALN
DIM FAST VIC AS BYTE
ONTSC:
    PRINT "6567R56A (Old NTSC)"
    VIC = 0
GOTO KEEPGOING
NTSC:
    PRINT "6567R8 (NTSC)"
    VIC = 1
GOTO KEEPGOING
PAL:
    PRINT "6569 (PAL)"
    VIC = 2
GOTO KEEPGOING
PALN:
    PRINT "6572 (PAL-N)"
    VIC = 3
KEEPGOING:
ASM
	;SID DETECTION ROUTINE

	;By SounDemon - Based on a tip from Dag Lem.
	;Put together by FTC after SounDemons instructions
	;...and tested by Rambones and Jeff.

	; - Don't run this routine on a badline

	sei		;No disturbing interrupts
	lda #$ff
	cmp $d012	;Don't run it on a badline.
	bne *-3

	;Detection itself starts here
	lda #$ff	;Set frequency in voice 3 to $ffff
	sta $d412	;...and set testbit (other bits don't matter) in VCREG3 ($d412) to disable oscillator
	sta $d40e
	sta $d40f
	lda #$20	;Sawtooth wave and gatebit OFF to start oscillator again.
	sta $d412
	lda $d41b	;Accu now has different value depending on sid model (6581=3/8580=2)
	sta {TEMP}
END ASM
PRINT "SID: ";
DIM FAST SID AS BYTE
IF TEMP = 3 THEN
    PRINT "6581"
    SID = 0
ELSE
    PRINT "8580"
    SID = 1
END IF
PRINT "Kernal: ";
TEMP = PEEK(65408)
DIM FAST KRN AS BYTE
IF TEMP = $AA THEN
    PRINT "Revision 1"
    KRN = 0
END IF
IF TEMP = $00 THEN
    PRINT "Revision 2"
    KRN = 1
END IF
IF TEMP = $03 THEN
    PRINT "Revision 3 (C64)"
    KRN = 2
END IF
IF TEMP = $43 THEN
    PRINT "SX-64"
    KRN = 3
END IF
IF TEMP = $64 THEN
    PRINT "Revision 3 (4064)"
    KRN = 4
END IF
PRINT "Model: ";
IF KRN = 3 THEN
    PRINT "SX-64 ";
    IF VIC = 2 THEN
        PRINT "(PAL)"
        GOTO FINISH
        ELSE
        PRINT "(NTSC)"
        GOTO FINISH
    END IF
END IF
IF KRN = 1 THEN
    PRINT "C64 (Old PAL)"
    GOTO FINISH
END IF
IF VIC = 0 THEN
    PRINT "C64 (Old NTSC)"
    GOTO FINISH
END IF
IF KRN = 2 THEN
    PRINT "C64";
    IF SID = 1 THEN PRINT "C";
    IF VIC = 2 THEN
        PRINT " (PAL)"
        GOTO FINISH
    ELSE
        IF VIC = 3 THEN
            PRINT " (DREAN)"
            GOTO FINISH
        END IF
        PRINT " (NTSC)"
        GOTO FINISH
    END IF
END IF
IF KRN = 4 THEN
    PRINT "4064 ";
    IF VIC = 2 THEN
        PRINT "(PAL)"
        GOTO FINISH
        ELSE
        PRINT "(NTSC)"
        GOTO FINISH
    END IF
END IF
PRINT "Unknown"
FINISH:
DO
LOOP UNTIL 0
