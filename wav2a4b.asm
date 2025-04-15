;-----------------------------------------------------------------------
;
; WAV2A4b - Play Sample 
; (c) 2025 GienekP
;
;-----------------------------------------------------------------------
PTR		= $A0
SMP1	= $A2
SMP2	= $A3
DMACTLS	= $022F
COLBAK  = $D01A
CONSOL  = $D01F
AUDF1	= $D200
AUDC1	= $D201
AUDF2	= $D202
AUDC2	= $D203
AUDF3	= $D204
AUDC3	= $D205
AUDF4	= $D206
AUDC4	= $D207
AUDCTL	= $D208
RANDOM	= $D20A
DMACTL  = $D400
WSYNC	= $D40A
VCOUNT  = $D40B
;-----------------------------------------------------------------------
		OPT h+
		ORG $2000
		RUN MAIN
;-----------------------------------------------------------------------
; Main function
MAIN	lda #$00
		sta COLBAK
		sta DMACTL
		sta DMACTLS
		sta AUDF1
		sta AUDF2
		sta AUDF3
		sta AUDF4
		sta AUDC1
		sta AUDC2
		sta AUDC3
		sta AUDC4
		sta AUDCTL
		lda #$17
		sta AUDC4
		sei
AGAIN	lda #<START
		sta PTR
		lda #>START
		sta PTR+1
		ldy #$00
		jsr WAIT
LOOP	lda (PTR),y
		tax
		and #$0F
		ora #$10
		sta SMP2
		sta WSYNC	; synchro
		txa
		lsr
		lsr
		lsr
		lsr
		ora #$10
		sta SMP1
		sta WSYNC	; synchro
		inc PTR
		bne @+
		inc PTR+1
@		lda PTR
		cmp STOP
		bne @+
		lda PTR+1
		cmp STOP+1
		bne @+
		jmp AGAIN
@		sta WSYNC	; synchro
		lda SMP1
		sta WSYNC	; synchro
		sta AUDC4	; ***
		sta WSYNC	; synchro
		txa
		sta WSYNC	; synchro
		sta COLBAK
		sta WSYNC	; synchro
		lda SMP2
		sta WSYNC	; synchro
		sta AUDC4	; ***
		jmp LOOP
		
WAIT	ldx #$20
WFRM	lda #$70
@		cmp VCOUNT
		bne @-
		lda #$72
@		cmp VCOUNT
		bne @-
		dex
		bne WFRM
INIT	rts
;-----------------------------------------------------------------------
		ORG $2100
STOP	dta a(END)
START
		:8 dta $77
END
;-----------------------------------------------------------------------
