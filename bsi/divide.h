// ассемблерный модуль 32-битного деления
// var1 = r19:r18:r17:r16, var2 = r23:r22:r21:r20
// teturn r19:r18:r17:r16 (остаток отбрасывается)

#pragma optimize=no_inline
static __x_z unsigned long DV4U(unsigned long var1, unsigned long var2)
{

// DD4Ux/DV4Ux=DRS4Ux.DRM4Ux DCNT4U=R24
// [R19-R16]/[R23-R20]=[R19-R16].[R15-R12]

asm (
"		clr	r12\n"
"		clr	r13\n"
"		clr	r14\n"
"		sub	r15, r15\n"
"		ldi	r24, 33\n"
"DV4U0_:\n"
"		rol	r16\n"
"		rol	r17\n"
"		rol	r18\n"
"		rol	r19\n"
"		dec	r24\n"
"		brne	DV4U1_\n"
"		ret\n"
"DV4U1_:\n"
"		rol	r12\n"
"		rol	r13\n"
"		rol	r14\n"
"		rol	r15\n"
"		sub	r12, r20\n"
"		sbc	r13, r21\n"
"		sbc	r14, r22\n"
"		sbc	r15, r23\n"
"		brcc	DV4U2_\n"
"		add	r12, r20\n"
"		adc	r13, r21\n"
"		adc	r14, r22\n"
"		adc	r15, r23\n"
"		clc\n"
"		rjmp	DV4U0_\n"
"DV4U2_:\n"
"		sec\n"
"		rjmp	DV4U0_" );

return 0;
}

