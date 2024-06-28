.include "zeropage.inc"

.global _detectsid
.global _detectregion
.importzp ptr1

.code

; methods grabbed from <https://codebase64.org>
_detectsid:
    SEI     ;No disturbing interrupts
    LDX #$00
    LDA #$FF
    CMP $D012   ;Don't run it on a badline.
    BNE *-3
    ;Detection itself starts here
    LDA #$FF    ;Set frequency in voice 3 to $ffff
    STA $D412   ;...and set testbit (other bits don't matter) in VCREG3 ($d412) to disable oscillator
    STA $D40E
    STA $D40F
    LDA #$20    ;Sawtooth wave and gatebit OFF to start oscillator again.
    STA $D412
    LDA $D41B   ;Accu now has different value depending on sid model (6581=3/8580=2)
    RTS

_detectregion:
    LDX #$00
    SEI
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
