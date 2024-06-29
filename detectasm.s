; TODO: Convert to C

.global _detect_region

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
