; TODO: Convert to C

.global _detect_region, _get_kernal_crc, _init_crc_tables
.importzp sreg

.code

_detect_region:
    SEI
    LDX #$00
w0: LDA $D012
w1: CMP $D012
    BEQ w1
    BMI w0
    AND #$03
    CMP #$03
    BNE detectionDone ; done for NTSC
    TAY
countCycles:
    INX
    LDA $D012
    BPL countCycles
    CPX #$5E  ; VICE values: PAL-N=$6C PAL=$50
	      ; so choose middle value $5E for check
    BCC isPAL
    INY ; is PAL-N
isPAL:
    TYA
detectionDone:
    RTS

PTR     = $1A
CRC0    = $C400
CRC1    = $C401
CRC2    = $C402
CRC3    = $C403

TAB0    = $C000
TAB1    = $C100
TAB2    = $C200
TAB3    = $C300

_get_kernal_crc:
    LDA #$FF
    STA CRC0
    STA CRC1
    STA CRC2
    STA CRC3

    LDA #$00
    STA PTR
    LDA #$E0
    STA PTR+1

LOOP:
    LDY #0
    LDA (PTR),Y
    JSR UPDCRC

    INC PTR
    BNE CHECK_END
    INC PTR+1

CHECK_END:
    LDA PTR+1
    BNE LOOP

    LDA CRC0
	EOR #$FF
	STA CRC0
    LDA CRC1
	EOR #$FF
	STA CRC1
    LDA CRC2
	EOR #$FF
	STA CRC2
    LDA CRC3
	EOR #$FF
	STA CRC3

    ; Format: A=LL, X=LH, sreg=HL, sreg+1=HH
    LDA CRC2
	STA sreg
    LDA CRC3
	STA sreg+1
    LDA CRC0
    LDX CRC1
    RTS

UPDCRC:
    EOR CRC0
    TAX
    LDA CRC1
	EOR TAB0,X
	STA CRC0
    LDA CRC2
	EOR TAB1,X
	STA CRC1
    LDA CRC3
	EOR TAB2,X
	STA CRC2
    LDA TAB3,X
	STA CRC3
    RTS

T0      = $C400     ; Low byte
T1      = $C401
T2      = $C402
T3      = $C403     ; High byte

POLY0   = $20
POLY1   = $83
POLY2   = $B8
POLY3   = $ED

_init_crc_tables:
    LDX #0
@entry_loop:
    STX T0
    LDA #0
    STA T1
	STA T2
	STA T3

    LDY #8
@bit_loop:
    LSR T3
	ROR T2
	ROR T1
	ROR T0
    BCC @no_xor

    LDA T0
	EOR #POLY0
	STA T0
    LDA T1
	EOR #POLY1
	STA T1
    LDA T2
	EOR #POLY2
	STA T2
    LDA T3
	EOR #POLY3
	STA T3
@no_xor:
    DEY
    BNE @bit_loop

    LDA T0
	STA TAB0,X
    LDA T1
	STA TAB1,X
    LDA T2
	STA TAB2,X
    LDA T3
	STA TAB3,X

    INX
    BNE @entry_loop
    RTS
