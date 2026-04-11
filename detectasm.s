; TODO: Convert to C

.global _detect_region, _kernal_checksum

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

zp_addr  = $FB
zp_chk1  = $FD        ; Low byte of sum
zp_chk2  = $FE        ; Mid byte (carry count)
zp_chk3  = $FF        ; EOR hash / High byte

_kernal_checksum:
    lda #0
    sta zp_chk1
    sta zp_chk2
    sta zp_chk3
    sta zp_addr

    lda #$E0
    sta zp_addr+1

    ldx #$20
    tay
    lda #0

chkloop:
    clc
    adc (zp_addr),y

    bcc chkskip

    inc zp_chk2
    bne chkskip

    pha
    tya
    eor zp_chk3
    sta zp_chk3
    pla

chkskip:
    iny
    bne chkloop

    inc zp_addr+1
    dex
    bne chkloop

    sta zp_chk1
    rts

;checksumtbl = *-5
;
;         .word rom_name1
;         .byte $bb,$d4,$fd     ;checksum 901227-01
;
;         .word rom_name2
;         .byte $8b,$c7,$0b     ;901227-02
;
;         .word rom_name3
;         .byte $b6,$c7,$0a     ;901227-03
;
;         .word rom_name4
;         .byte $ea,$c9,$09     ;901227-03-DK
;
;         .word rom_name5
;         .byte $b3,$c5,$ca     ;swedish-02
;
;         .word rom_name6
;         .byte $b0,$c5,$c9     ;swedish-03
;
;         .word rom_name7
;         .byte $bd,$c7,$0b     ;sx.251104-04
;
;         .word rom_name8
;         .byte $ba,$c7,$88     ;sx64-scand.bin
;
;         .word rom_name9
;         .byte $5c,$d1,$83     ;906145-02-jp
;
;         .word rom_name10
;         .byte $56,$b5,$ca     ;c64gs
;
;         .word rom_name11
;         .byte $68,$a9,$e5     ;Jiffydos6.01
;
;         .word $ffff
;         .byte $ff,$ff,$ff     ;END
