	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 2
	.globl	__Z13array_builderPi            ; -- Begin function _Z13array_builderPi
	.p2align	2
__Z13array_builderPi:                   ; @_Z13array_builderPi
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	x0, [sp, #8]
	str	wzr, [sp, #4]
	b	LBB0_1
LBB0_1:                                 ; =>This Inner Loop Header: Depth=1
	ldr	w8, [sp, #4]
	subs	w8, w8, #20
	cset	w8, ge
	tbnz	w8, #0, LBB0_4
	b	LBB0_2
LBB0_2:                                 ;   in Loop: Header=BB0_1 Depth=1
	ldr	w8, [sp, #4]
	ldr	w9, [sp, #4]
	mul	w8, w8, w9
	ldr	x9, [sp, #8]
	ldrsw	x10, [sp, #4]
	str	w8, [x9, x10, lsl #2]
	b	LBB0_3
LBB0_3:                                 ;   in Loop: Header=BB0_1 Depth=1
	ldr	w8, [sp, #4]
	add	w8, w8, #1
	str	w8, [sp, #4]
	b	LBB0_1
LBB0_4:
	ldr	x0, [sp, #8]
	add	sp, sp, #16
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #48
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	stur	wzr, [x29, #-4]
	mov	x0, #80                         ; =0x50
	bl	__Znam
	str	x0, [sp, #16]
	ldr	x0, [sp, #16]
	bl	__Z13array_builderPi
	ldr	x8, [sp, #16]
	str	x8, [sp, #8]                    ; 8-byte Folded Spill
	subs	x8, x8, #0
	cset	w8, eq
	tbnz	w8, #0, LBB1_2
	b	LBB1_1
LBB1_1:
	ldr	x0, [sp, #8]                    ; 8-byte Folded Reload
	bl	__ZdaPv
	b	LBB1_2
LBB1_2:
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #48
	ret
	.cfi_endproc
                                        ; -- End function
.subsections_via_symbols
