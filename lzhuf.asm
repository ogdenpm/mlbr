; Listing generated by Microsoft (R) Optimizing Compiler Version 19.24.28316.0 

	TITLE	D:\Projects\mlbr\lzhuf.c
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_textsize
PUBLIC	_codesize
PUBLIC	_printcount
PUBLIC	_d_code
PUBLIC	_d_len
PUBLIC	_getbuf
PUBLIC	_getlen
PUBLIC	_putbuf
PUBLIC	_putlen
_DATA	SEGMENT
COMM	_infile:DWORD
COMM	_outfile:DWORD
_DATA	ENDS
_BSS	SEGMENT
_textsize DD	01H DUP (?)
_codesize DD	01H DUP (?)
_printcount DD	01H DUP (?)
_BSS	ENDS
_DATA	SEGMENT
COMM	_text_buf:BYTE:083bH
COMM	_oldver:BYTE
COMM	_freq:DWORD:0276H
COMM	_prnt:DWORD:03b0H
COMM	_son:DWORD:0275H
_DATA	ENDS
_BSS	SEGMENT
_getbuf	DD	01H DUP (?)
_getlen	DB	01H DUP (?)
	ALIGN	4

_putbuf	DD	01H DUP (?)
_putlen	DB	01H DUP (?)
_BSS	ENDS
_DATA	SEGMENT
COMM	_code:DWORD
COMM	_len:DWORD
_DATA	ENDS
_DATA	SEGMENT
_d_code	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	01H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	02H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	09H
	DB	09H
	DB	09H
	DB	09H
	DB	09H
	DB	09H
	DB	09H
	DB	09H
	DB	0aH
	DB	0aH
	DB	0aH
	DB	0aH
	DB	0aH
	DB	0aH
	DB	0aH
	DB	0aH
	DB	0bH
	DB	0bH
	DB	0bH
	DB	0bH
	DB	0bH
	DB	0bH
	DB	0bH
	DB	0bH
	DB	0cH
	DB	0cH
	DB	0cH
	DB	0cH
	DB	0dH
	DB	0dH
	DB	0dH
	DB	0dH
	DB	0eH
	DB	0eH
	DB	0eH
	DB	0eH
	DB	0fH
	DB	0fH
	DB	0fH
	DB	0fH
	DB	010H
	DB	010H
	DB	010H
	DB	010H
	DB	011H
	DB	011H
	DB	011H
	DB	011H
	DB	012H
	DB	012H
	DB	012H
	DB	012H
	DB	013H
	DB	013H
	DB	013H
	DB	013H
	DB	014H
	DB	014H
	DB	014H
	DB	014H
	DB	015H
	DB	015H
	DB	015H
	DB	015H
	DB	016H
	DB	016H
	DB	016H
	DB	016H
	DB	017H
	DB	017H
	DB	017H
	DB	017H
	DB	018H
	DB	018H
	DB	019H
	DB	019H
	DB	01aH
	DB	01aH
	DB	01bH
	DB	01bH
	DB	01cH
	DB	01cH
	DB	01dH
	DB	01dH
	DB	01eH
	DB	01eH
	DB	01fH
	DB	01fH
	DB	020H
	DB	020H
	DB	021H
	DB	021H
	DB	022H
	DB	022H
	DB	023H
	DB	023H
	DB	024H
	DB	024H
	DB	025H
	DB	025H
	DB	026H
	DB	026H
	DB	027H
	DB	027H
	DB	028H
	DB	028H
	DB	029H
	DB	029H
	DB	02aH
	DB	02aH
	DB	02bH
	DB	02bH
	DB	02cH
	DB	02cH
	DB	02dH
	DB	02dH
	DB	02eH
	DB	02eH
	DB	02fH
	DB	02fH
	DB	030H
	DB	031H
	DB	032H
	DB	033H
	DB	034H
	DB	035H
	DB	036H
	DB	037H
	DB	038H
	DB	039H
	DB	03aH
	DB	03bH
	DB	03cH
	DB	03dH
	DB	03eH
	DB	03fH
_d_len	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	03H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	04H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	05H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	06H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	07H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
	DB	08H
_DATA	ENDS
PUBLIC	___local_stdio_printf_options
PUBLIC	__vfprintf_l
PUBLIC	_printf
PUBLIC	_Error
PUBLIC	_GetBit
PUBLIC	_GetByte
PUBLIC	_startHuff
PUBLIC	_reconst
PUBLIC	_update
PUBLIC	_DecodeChar
PUBLIC	_DecodePosition
PUBLIC	_unlzh
PUBLIC	??_C@_04EINJHBMM@?6?$CFs?6@			; `string'
PUBLIC	__xmm@00000001000000010000000100000001
PUBLIC	__xmm@00000003000000020000000100000000
PUBLIC	__xmm@00000275000002750000027500000275
EXTRN	___acrt_iob_func:PROC
EXTRN	_getc:PROC
EXTRN	___stdio_common_vfprintf:PROC
EXTRN	_exit:PROC
EXTRN	_outU8:PROC
EXTRN	_memset:PROC
_DATA	SEGMENT
COMM	?_OptionsStorage@?1??__local_stdio_printf_options@@9@9:QWORD							; `__local_stdio_printf_options'::`2'::_OptionsStorage
_DATA	ENDS
;	COMDAT __xmm@00000275000002750000027500000275
CONST	SEGMENT
__xmm@00000275000002750000027500000275 DB 'u', 02H, 00H, 00H, 'u', 02H, 00H
	DB	00H, 'u', 02H, 00H, 00H, 'u', 02H, 00H, 00H
CONST	ENDS
;	COMDAT __xmm@00000003000000020000000100000000
CONST	SEGMENT
__xmm@00000003000000020000000100000000 DB 00H, 00H, 00H, 00H, 01H, 00H, 00H
	DB	00H, 02H, 00H, 00H, 00H, 03H, 00H, 00H, 00H
CONST	ENDS
;	COMDAT __xmm@00000001000000010000000100000001
CONST	SEGMENT
__xmm@00000001000000010000000100000001 DB 01H, 00H, 00H, 00H, 01H, 00H, 00H
	DB	00H, 01H, 00H, 00H, 00H, 01H, 00H, 00H, 00H
CONST	ENDS
;	COMDAT ??_C@_04EINJHBMM@?6?$CFs?6@
CONST	SEGMENT
??_C@_04EINJHBMM@?6?$CFs?6@ DB 0aH, '%s', 0aH, 00H	; `string'
CONST	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _unlzh
_TEXT	SEGMENT
_c$1$ = -10						; size = 1
_j$1$ = -9						; size = 1
_c$1$ = -8						; size = 4
_i$2$ = -4						; size = 4
_j$1$ = -4						; size = 4
_content$ = 8						; size = 4
_unlzh	PROC						; COMDAT

; 313  : bool unlzh(content_t *content) { /* Decoding/Uncompressing */

	sub	esp, 12					; 0000000cH
	push	ebx
	push	ebp
	push	esi
	push	edi

; 314  : 	int  i, j, k, r, c;
; 315  : 
; 316  : 	// TO-DO read info bytes
; 317  : 
; 318  : 	startHuff();

	call	_startHuff

; 319  : 	r = LZ_N - LZ_F;

	mov	esi, 1988				; 000007c4H

; 320  : 	memset(text_buf, ' ', r);

	push	esi
	push	32					; 00000020H
	push	OFFSET _text_buf
	call	_memset
	add	esp, 12					; 0000000cH
$LN48@unlzh:

; 279  : 	c = son[LZ_R];

	mov	ebp, 63					; 0000003fH
	npad	10
$LL2@unlzh:
	mov	ebx, DWORD PTR _son+2512

; 280  : 
; 281  : 	/*
; 282  : 	 * start searching tree from the root to leaves.
; 283  : 	 * choose node #(son[]) if input bit == 0
; 284  : 	 * else choose #(son[]+1) (input bit == 1)
; 285  : 	 */
; 286  : 	while (c < LZ_T) {

	cmp	ebx, 629				; 00000275H
	jae	SHORT $LN12@unlzh
	npad	2
$LL11@unlzh:

; 287  : 		c += GetBit();

	call	_GetBit
	add	ebx, eax

; 288  : 		c = son[c];

	mov	ebx, DWORD PTR _son[ebx*4]
	cmp	ebx, 629				; 00000275H
	jb	SHORT $LL11@unlzh
$LN12@unlzh:

; 289  : 	}
; 290  : 	c -= LZ_T;

	sub	ebx, 629				; 00000275H

; 291  : 	update(c);

	push	ebx
	mov	DWORD PTR _c$1$[esp+32], ebx
	call	_update
	add	esp, 4

; 321  : 
; 322  : 	while ((c = DecodeChar()) != EOF_CODE) {

	cmp	ebx, 256				; 00000100H
	je	$LN3@unlzh

; 323  : 		if (c < EOF_CODE) {

	jge	SHORT $LN25@unlzh

; 324  : 			outU8(c, content);

	push	DWORD PTR _content$[esp+24]
	push	ebx
	call	_outU8

; 325  : 			text_buf[r++] = c;

	mov	BYTE PTR _text_buf[esi], bl
	add	esp, 8
	inc	esi

; 326  : 			r %= LZ_N;

	and	esi, -2147481601			; 800007ffH
	jns	SHORT $LL2@unlzh
	dec	esi
	or	esi, -2048				; fffff800H
	inc	esi

; 327  : 		} else {

	jmp	SHORT $LL2@unlzh
$LN25@unlzh:

; 165  : 	while (getlen <= 8) {

	mov	cl, BYTE PTR _getlen
	cmp	cl, 8
	ja	SHORT $LN43@unlzh
	npad	8
$LL19@unlzh:

; 166  : 		if ((i = getc(infile)) < 0) i = 0;

	push	DWORD PTR _infile
	call	_getc

; 167  : 		getbuf |= i << (8 - getlen);

	mov	bl, BYTE PTR _getlen
	mov	ecx, 8
	movzx	edx, bl
	add	esp, 4
	sub	ecx, edx

; 168  : 		getlen += 8;

	add	bl, 8
	mov	edx, DWORD PTR _getbuf
	shl	eax, cl
	or	edx, eax
	mov	BYTE PTR _getlen, bl
	mov	DWORD PTR _getbuf, edx
	cmp	bl, 8
	jbe	SHORT $LL19@unlzh

; 165  : 	while (getlen <= 8) {

	mov	cl, bl
	jmp	SHORT $LN20@unlzh
$LN43@unlzh:
	mov	edx, DWORD PTR _getbuf
$LN20@unlzh:

; 169  : 	}
; 170  : 	i = getbuf;

	mov	eax, edx

; 171  : 	getbuf <<= 8;
; 172  : 	getlen -= 8;

	add	cl, 248					; 000000f8H
	shl	eax, 8
	mov	DWORD PTR _getbuf, eax

; 173  : 	return i >> 8;

	shr	edx, 8

; 300  : 	c = d_code[i] << (5 + oldver);		// 5 or 6 for 1.x

	movzx	eax, dl

; 172  : 	getlen -= 8;

	mov	BYTE PTR _getlen, cl

; 300  : 	c = d_code[i] << (5 + oldver);		// 5 or 6 for 1.x

	movzx	ecx, BYTE PTR _oldver
	add	ecx, 5

; 173  : 	return i >> 8;

	mov	DWORD PTR _i$2$[esp+28], edx

; 300  : 	c = d_code[i] << (5 + oldver);		// 5 or 6 for 1.x

	mov	bl, BYTE PTR _d_code[eax]

; 301  : 
; 302  : 	/* input lower 6 bits directly */
; 303  : 	j = d_len[i] - (3 - oldver);		// 3 or 2 for 1.x

	mov	al, BYTE PTR _d_len[eax]
	add	al, BYTE PTR _oldver
	mov	BYTE PTR _c$1$[esp+28], bl
	shl	BYTE PTR _c$1$[esp+28], cl
	sub	al, 3

; 304  : 
; 305  : 	while (j--)

	mov	ebx, DWORD PTR _c$1$[esp+28]
	mov	BYTE PTR _j$1$[esp+28], al
	je	SHORT $LN30@unlzh
	mov	bl, al
$LL15@unlzh:

; 306  : 		i = (i << 1) + GetBit();

	call	_GetBit
	mov	edx, DWORD PTR _i$2$[esp+28]
	add	dl, dl
	add	dl, al
	mov	DWORD PTR _i$2$[esp+28], edx
	add	bl, 255					; 000000ffH
	jne	SHORT $LL15@unlzh
	mov	ebx, DWORD PTR _c$1$[esp+28]
$LN30@unlzh:

; 307  : 
; 308  : 	return c | (i & (oldver ? 0x3f : 0x1f));	// 0x1f or 0x3f for 1.x

	cmp	BYTE PTR _oldver, 0
	mov	ecx, 31					; 0000001fH
	movzx	eax, dl
	cmovne	ecx, ebp
	mov	ebp, esi
	and	ecx, eax
	movzx	eax, BYTE PTR _c$1$[esp+28]
	or	ecx, eax
	sub	ebp, ecx

; 328  : 			i = (r - DecodePosition() - 1) % LZ_N;

	dec	ebp
	and	ebp, -2147481601			; 800007ffH
	jns	SHORT $LN45@unlzh
	dec	ebp
	or	ebp, -2048				; fffff800H
	inc	ebp
$LN45@unlzh:

; 329  : 			j = c - EOF_CODE + THRESHOLD;

	lea	eax, DWORD PTR [ebx-254]

; 308  : 	return c | (i & (oldver ? 0x3f : 0x1f));	// 0x1f or 0x3f for 1.x

	xor	edi, edi

; 329  : 			j = c - EOF_CODE + THRESHOLD;

	mov	DWORD PTR _j$1$[esp+28], eax
	npad	5
$LL6@unlzh:

; 331  : 				c = text_buf[(i + k) % LZ_N];

	lea	eax, DWORD PTR [edi+ebp]
	and	eax, -2147481601			; 800007ffH
	jns	SHORT $LN46@unlzh
	dec	eax
	or	eax, -2048				; fffff800H
	inc	eax
$LN46@unlzh:
	mov	bl, BYTE PTR _text_buf[eax]

; 332  : 				outU8(c, content);

	push	DWORD PTR _content$[esp+24]
	movzx	eax, bl
	push	eax
	call	_outU8

; 333  : 				text_buf[r++] = c;

	mov	BYTE PTR _text_buf[esi], bl
	add	esp, 8
	inc	esi

; 334  : 				r %= LZ_N;

	and	esi, -2147481601			; 800007ffH
	jns	SHORT $LN47@unlzh
	dec	esi
	or	esi, -2048				; fffff800H
	inc	esi
$LN47@unlzh:

; 330  : 			for (k = 0; k < j; k++) {

	inc	edi
	cmp	edi, DWORD PTR _j$1$[esp+28]
	jl	SHORT $LL6@unlzh

; 335  : 			}
; 336  : 		}
; 337  : 	}

	jmp	$LN48@unlzh
$LN3@unlzh:
	pop	edi

; 338  : }

	pop	esi
	pop	ebp
	pop	ebx
	add	esp, 12					; 0000000cH
	ret	0
_unlzh	ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _DecodePosition
_TEXT	SEGMENT
_j$1$ = -2						; size = 1
_c$1$ = -1						; size = 1
_DecodePosition PROC					; COMDAT

; 295  : int DecodePosition() {

	push	ecx

; 165  : 	while (getlen <= 8) {

	mov	cl, BYTE PTR _getlen

; 295  : int DecodePosition() {

	push	ebx

; 165  : 	while (getlen <= 8) {

	cmp	cl, 8
	ja	SHORT $LN19@DecodePosi
	npad	3
$LL6@DecodePosi:

; 166  : 		if ((i = getc(infile)) < 0) i = 0;

	push	DWORD PTR _infile
	call	_getc

; 167  : 		getbuf |= i << (8 - getlen);

	movzx	edx, BYTE PTR _getlen
	mov	ecx, 8
	mov	ebx, DWORD PTR _getbuf
	sub	ecx, edx
	shl	eax, cl
	add	esp, 4

; 168  : 		getlen += 8;

	mov	cl, BYTE PTR _getlen
	or	ebx, eax
	add	cl, 8
	mov	DWORD PTR _getbuf, ebx
	mov	BYTE PTR _getlen, cl
	cmp	cl, 8
	jbe	SHORT $LL6@DecodePosi

; 165  : 	while (getlen <= 8) {

	jmp	SHORT $LN7@DecodePosi
$LN19@DecodePosi:
	mov	ebx, DWORD PTR _getbuf
$LN7@DecodePosi:

; 296  : 	uint8_t i, j, c;
; 297  : 
; 298  : 	/* decode upper 6 bits from given table */
; 299  : 	i = GetByte();
; 300  : 	c = d_code[i] << (5 + oldver);		// 5 or 6 for 1.x

	mov	dl, BYTE PTR _oldver

; 170  : 	i = getbuf;

	mov	eax, ebx
	shl	eax, 8

; 171  : 	getbuf <<= 8;
; 172  : 	getlen -= 8;

	add	cl, 248					; 000000f8H
	mov	DWORD PTR _getbuf, eax

; 173  : 	return i >> 8;

	shr	ebx, 8

; 296  : 	uint8_t i, j, c;
; 297  : 
; 298  : 	/* decode upper 6 bits from given table */
; 299  : 	i = GetByte();
; 300  : 	c = d_code[i] << (5 + oldver);		// 5 or 6 for 1.x

	movzx	eax, bl

; 172  : 	getlen -= 8;

	mov	BYTE PTR _getlen, cl

; 296  : 	uint8_t i, j, c;
; 297  : 
; 298  : 	/* decode upper 6 bits from given table */
; 299  : 	i = GetByte();
; 300  : 	c = d_code[i] << (5 + oldver);		// 5 or 6 for 1.x

	movzx	ecx, dl
	add	ecx, 5
	mov	dh, BYTE PTR _d_code[eax]

; 301  : 
; 302  : 	/* input lower 6 bits directly */
; 303  : 	j = d_len[i] - (3 - oldver);		// 3 or 2 for 1.x

	mov	al, BYTE PTR _d_len[eax]
	shl	dh, cl
	add	al, dl
	mov	BYTE PTR _c$1$[esp+8], dh
	sub	al, 3

; 304  : 
; 305  : 	while (j--)

	je	SHORT $LN13@DecodePosi
$LL2@DecodePosi:
	dec	al
	mov	BYTE PTR _j$1$[esp+8], al

; 306  : 		i = (i << 1) + GetBit();

	call	_GetBit
	add	bl, bl
	add	bl, al
	mov	al, BYTE PTR _j$1$[esp+8]
	test	al, al
	jne	SHORT $LL2@DecodePosi
	mov	dl, BYTE PTR _oldver
	mov	dh, BYTE PTR _c$1$[esp+8]
$LN13@DecodePosi:

; 307  : 
; 308  : 	return c | (i & (oldver ? 0x3f : 0x1f));	// 0x1f or 0x3f for 1.x

	test	dl, dl
	mov	ecx, 63					; 0000003fH
	mov	eax, 31					; 0000001fH
	cmovne	eax, ecx
	movzx	ecx, bl
	and	eax, ecx
	movzx	ecx, dh
	or	eax, ecx
	pop	ebx

; 309  : }

	pop	ecx
	ret	0
_DecodePosition ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _DecodeChar
_TEXT	SEGMENT
_DecodeChar PROC					; COMDAT

; 276  : int DecodeChar() {

	push	esi

; 277  : 	unsigned c;
; 278  : 
; 279  : 	c = son[LZ_R];

	mov	esi, DWORD PTR _son+2512

; 280  : 
; 281  : 	/*
; 282  : 	 * start searching tree from the root to leaves.
; 283  : 	 * choose node #(son[]) if input bit == 0
; 284  : 	 * else choose #(son[]+1) (input bit == 1)
; 285  : 	 */
; 286  : 	while (c < LZ_T) {

	cmp	esi, 629				; 00000275H
	jae	SHORT $LN3@DecodeChar
	npad	1
$LL2@DecodeChar:

; 287  : 		c += GetBit();

	call	_GetBit
	add	esi, eax

; 288  : 		c = son[c];

	mov	esi, DWORD PTR _son[esi*4]
	cmp	esi, 629				; 00000275H
	jb	SHORT $LL2@DecodeChar
$LN3@DecodeChar:

; 289  : 	}
; 290  : 	c -= LZ_T;

	sub	esi, 629				; 00000275H

; 291  : 	update(c);

	push	esi
	call	_update
	add	esp, 4

; 292  : 	return c;

	mov	eax, esi
	pop	esi

; 293  : }

	ret	0
_DecodeChar ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _update
_TEXT	SEGMENT
_j$1$ = -8						; size = 4
tv683 = -4						; size = 4
_c$ = 8							; size = 4
_update	PROC						; COMDAT

; 239  : void update(int c) {

	sub	esp, 8

; 240  : 	int i, j, k, l;
; 241  : 
; 242  : 	if (freq[LZ_R] == MAX_FREQ)

	cmp	DWORD PTR _freq+2512, 32768		; 00008000H
	push	esi
	jne	$LN23@update

; 207  : 	j = 0;

	xor	ecx, ecx
	xor	edx, edx
	npad	8
$LL15@update:

; 209  : 		if (son[i] >= LZ_T) {

	mov	esi, DWORD PTR _son[ecx]
	cmp	esi, 629				; 00000275H
	jl	SHORT $LN13@update

; 210  : 			freq[j] = (freq[i] + 1) / 2;

	mov	eax, DWORD PTR _freq[ecx]
	inc	eax

; 211  : 			son[j] = son[i];

	mov	DWORD PTR _son[edx], esi
	shr	eax, 1
	mov	DWORD PTR _freq[edx], eax

; 212  : 			j++;

	add	edx, 4
$LN13@update:

; 208  : 	for (i = 0; i < LZ_T; i++) {

	add	ecx, 4
	cmp	ecx, 2516				; 000009d4H
	jl	SHORT $LL15@update

; 213  : 		}
; 214  : 	}
; 215  : 	/* make a tree : first, connect children nodes */
; 216  : 	for (i = 0, j = N_CHAR; j < LZ_T; i += 2, j++) {

	push	ebx
	push	ebp
	push	edi
	mov	edi, OFFSET _freq+1256
	mov	edx, 1260				; 000004ecH
	xor	ebp, ebp
	mov	eax, 315				; 0000013bH
	sub	edx, edi
	mov	DWORD PTR _j$1$[esp+24], eax
	mov	DWORD PTR tv683[esp+24], edx
	npad	1
$LL18@update:

; 217  : 		k = i + 1;
; 218  : 		f = freq[i] + freq[k];

	mov	esi, DWORD PTR _freq[ebp*4+4]

; 219  : 		for (k = j; f < freq[k - 1]; k--) {

	mov	ecx, eax
	add	esi, DWORD PTR _freq[ebp*4]
	mov	eax, DWORD PTR [edi]
	cmp	esi, eax
	jae	SHORT $LN20@update
	lea	ebx, DWORD PTR [edi+4]
	add	edx, edi
	npad	5
$LL21@update:
	dec	ecx

; 220  : 			freq[k] = freq[k - 1];

	mov	DWORD PTR [ebx], eax

; 221  : 			son[k] = son[k - 1];

	mov	eax, DWORD PTR _son[edx-4]
	mov	DWORD PTR _son[edx], eax
	lea	edx, DWORD PTR [ecx*4]
	mov	eax, DWORD PTR _freq[edx-4]
	lea	ebx, DWORD PTR _freq[edx]
	cmp	esi, eax
	jb	SHORT $LL21@update
	mov	edx, DWORD PTR tv683[esp+24]
$LN20@update:

; 213  : 		}
; 214  : 	}
; 215  : 	/* make a tree : first, connect children nodes */
; 216  : 	for (i = 0, j = N_CHAR; j < LZ_T; i += 2, j++) {

	mov	eax, DWORD PTR _j$1$[esp+24]
	add	edi, 4

; 222  : 		}
; 223  : 		freq[k] = f;
; 224  : 		son[k] = i;

	mov	DWORD PTR _son[ecx*4], ebp
	inc	eax
	add	ebp, 2
	mov	DWORD PTR _freq[ecx*4], esi
	mov	DWORD PTR _j$1$[esp+24], eax
	cmp	edi, OFFSET _freq+2512
	jl	SHORT $LL18@update

; 225  : 
; 226  : 	}
; 227  : 	/* connect parent nodes */
; 228  : 	for (i = 0; i < LZ_T; i++) {

	pop	edi
	pop	ebp
	xor	eax, eax
	pop	ebx
	npad	12
$LL24@update:

; 229  : 		if ((k = son[i]) >= LZ_T)

	mov	ecx, DWORD PTR _son[eax*4]
	cmp	ecx, 629				; 00000275H
	jge	SHORT $LN22@update

; 230  : 			prnt[k] = i;
; 231  : 		else
; 232  : 			prnt[k] = prnt[k + 1] = i;

	mov	DWORD PTR _prnt[ecx*4+4], eax
$LN22@update:

; 225  : 
; 226  : 	}
; 227  : 	/* connect parent nodes */
; 228  : 	for (i = 0; i < LZ_T; i++) {

	mov	DWORD PTR _prnt[ecx*4], eax
	inc	eax
	cmp	eax, 629				; 00000275H
	jl	SHORT $LL24@update
$LN23@update:

; 243  : 		reconst();
; 244  : 
; 245  : 	c = prnt[c + LZ_T];

	mov	eax, DWORD PTR _c$[esp+8]
	mov	edx, DWORD PTR _prnt[eax*4+2516]
$LL4@update:

; 246  : 	do {
; 247  : 		k = ++freq[c];

	inc	DWORD PTR _freq[edx*4]

; 248  : 
; 249  : 		/* swap nodes to keep the tree freq-ordered */
; 250  : 		if (k > freq[l = c + 1]) {

	lea	eax, DWORD PTR [edx+1]
	mov	esi, DWORD PTR _freq[edx*4]
	cmp	esi, DWORD PTR _freq[eax*4]
	jbe	SHORT $LN2@update
	npad	6
$LL5@update:

; 251  : 			while (k > freq[++l]);

	inc	eax
	cmp	esi, DWORD PTR _freq[eax*4]
	ja	SHORT $LL5@update

; 252  : 			l--;
; 253  : 			freq[c] = freq[l];

	mov	ecx, DWORD PTR _freq[eax*4-4]
	dec	eax
	mov	DWORD PTR _freq[edx*4], ecx

; 254  : 			freq[l] = k;

	mov	DWORD PTR _freq[eax*4], esi

; 255  : 
; 256  : 			i = son[c];

	mov	esi, DWORD PTR _son[edx*4]

; 257  : 			prnt[i] = l;

	mov	DWORD PTR _prnt[esi*4], eax

; 258  : 			if (i < LZ_T)

	cmp	esi, 629				; 00000275H
	jge	SHORT $LN9@update

; 259  : 				prnt[i + 1] = l;

	mov	DWORD PTR _prnt[esi*4+4], eax
$LN9@update:

; 260  : 
; 261  : 			j = son[l];

	mov	ecx, DWORD PTR _son[eax*4]

; 262  : 			son[l] = i;

	mov	DWORD PTR _son[eax*4], esi

; 263  : 
; 264  : 			prnt[j] = c;

	mov	DWORD PTR _prnt[ecx*4], edx

; 265  : 			if (j < LZ_T)

	cmp	ecx, 629				; 00000275H
	jge	SHORT $LN10@update

; 266  : 				prnt[j + 1] = c;

	mov	DWORD PTR _prnt[ecx*4+4], edx
$LN10@update:

; 267  : 			son[c] = j;

	mov	DWORD PTR _son[edx*4], ecx

; 268  : 
; 269  : 			c = l;

	mov	edx, eax
$LN2@update:

; 270  : 		}
; 271  : 	} while ((c = prnt[c]) != 0);	/* do it until reaching the root */

	mov	edx, DWORD PTR _prnt[edx*4]
	test	edx, edx
	jne	$LL4@update

; 272  : }

	pop	esi
	add	esp, 8
	ret	0
_update	ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _reconst
_TEXT	SEGMENT
_j$1$ = -8						; size = 4
tv597 = -4						; size = 4
_reconst PROC						; COMDAT

; 202  : void reconst() {

	sub	esp, 8

; 203  : 	int i, j, k;
; 204  : 	unsigned f;
; 205  : 
; 206  : 	/* halven cumulative freq for leaf nodes */
; 207  : 	j = 0;

	xor	ecx, ecx
	xor	edx, edx
	push	ebx
	push	ebp
	push	esi
	push	edi
	npad	5
$LL4@reconst:

; 209  : 		if (son[i] >= LZ_T) {

	mov	esi, DWORD PTR _son[ecx]
	cmp	esi, 629				; 00000275H
	jl	SHORT $LN2@reconst

; 210  : 			freq[j] = (freq[i] + 1) / 2;

	mov	eax, DWORD PTR _freq[ecx]
	inc	eax

; 211  : 			son[j] = son[i];

	mov	DWORD PTR _son[edx], esi
	shr	eax, 1
	mov	DWORD PTR _freq[edx], eax

; 212  : 			j++;

	add	edx, 4
$LN2@reconst:

; 208  : 	for (i = 0; i < LZ_T; i++) {

	add	ecx, 4
	cmp	ecx, 2516				; 000009d4H
	jl	SHORT $LL4@reconst

; 213  : 		}
; 214  : 	}
; 215  : 	/* make a tree : first, connect children nodes */
; 216  : 	for (i = 0, j = N_CHAR; j < LZ_T; i += 2, j++) {

	mov	edi, OFFSET _freq+1256
	mov	edx, 1260				; 000004ecH
	xor	ebp, ebp
	mov	eax, 315				; 0000013bH
	sub	edx, edi
	mov	DWORD PTR _j$1$[esp+24], eax
	mov	DWORD PTR tv597[esp+24], edx
	npad	4
$LL7@reconst:

; 217  : 		k = i + 1;
; 218  : 		f = freq[i] + freq[k];

	mov	esi, DWORD PTR _freq[ebp*4+4]

; 219  : 		for (k = j; f < freq[k - 1]; k--) {

	mov	ecx, eax
	add	esi, DWORD PTR _freq[ebp*4]
	mov	eax, DWORD PTR [edi]
	cmp	esi, eax
	jae	SHORT $LN9@reconst
	lea	ebx, DWORD PTR [edi+4]
	add	edx, edi
	npad	5
$LL10@reconst:
	dec	ecx

; 220  : 			freq[k] = freq[k - 1];

	mov	DWORD PTR [ebx], eax

; 221  : 			son[k] = son[k - 1];

	mov	eax, DWORD PTR _son[edx-4]
	mov	DWORD PTR _son[edx], eax
	lea	edx, DWORD PTR [ecx*4]
	mov	eax, DWORD PTR _freq[edx-4]
	lea	ebx, DWORD PTR _freq[edx]
	cmp	esi, eax
	jb	SHORT $LL10@reconst
	mov	edx, DWORD PTR tv597[esp+24]
$LN9@reconst:

; 213  : 		}
; 214  : 	}
; 215  : 	/* make a tree : first, connect children nodes */
; 216  : 	for (i = 0, j = N_CHAR; j < LZ_T; i += 2, j++) {

	mov	eax, DWORD PTR _j$1$[esp+24]
	add	edi, 4

; 222  : 		}
; 223  : 		freq[k] = f;
; 224  : 		son[k] = i;

	mov	DWORD PTR _son[ecx*4], ebp
	inc	eax
	add	ebp, 2
	mov	DWORD PTR _freq[ecx*4], esi
	mov	DWORD PTR _j$1$[esp+24], eax
	cmp	edi, OFFSET _freq+2512
	jl	SHORT $LL7@reconst

; 225  : 
; 226  : 	}
; 227  : 	/* connect parent nodes */
; 228  : 	for (i = 0; i < LZ_T; i++) {

	pop	edi
	pop	esi
	pop	ebp
	xor	eax, eax
	pop	ebx
	npad	11
$LL13@reconst:

; 229  : 		if ((k = son[i]) >= LZ_T)

	mov	ecx, DWORD PTR _son[eax*4]
	cmp	ecx, 629				; 00000275H
	jge	SHORT $LN11@reconst

; 230  : 			prnt[k] = i;
; 231  : 		else
; 232  : 			prnt[k] = prnt[k + 1] = i;

	mov	DWORD PTR _prnt[ecx*4+4], eax
$LN11@reconst:

; 225  : 
; 226  : 	}
; 227  : 	/* connect parent nodes */
; 228  : 	for (i = 0; i < LZ_T; i++) {

	mov	DWORD PTR _prnt[ecx*4], eax
	inc	eax
	cmp	eax, 629				; 00000275H
	jl	SHORT $LL13@reconst

; 233  : 	}
; 234  : }

	add	esp, 8
	ret	0
_reconst ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _startHuff
_TEXT	SEGMENT
_startHuff PROC						; COMDAT

; 183  : 	int i, j;
; 184  : 
; 185  : 	for (i = 0; i < N_CHAR; i++) {

	movaps	xmm2, XMMWORD PTR __xmm@00000003000000020000000100000000
	xor	edx, edx
	movaps	xmm3, XMMWORD PTR __xmm@00000001000000010000000100000001
	movaps	xmm4, XMMWORD PTR __xmm@00000275000002750000027500000275
	npad	9
$LL4@startHuff:

; 186  : 		freq[i] = 1;
; 187  : 		son[i] = i + LZ_T;

	movd	xmm0, edx
	lea	eax, DWORD PTR [edx+4]
	pshufd	xmm1, xmm0, 0
	movaps	xmm0, xmm4
	paddd	xmm1, xmm2
	paddd	xmm0, xmm1
	movups	XMMWORD PTR _son[edx*4], xmm0

; 188  : 		prnt[i + LZ_T] = i;

	movups	XMMWORD PTR _prnt[edx*4+2516], xmm1
	movd	xmm0, eax
	lea	eax, DWORD PTR [edx+8]
	pshufd	xmm1, xmm0, 0
	paddd	xmm1, xmm2
	movaps	xmm0, xmm1
	paddd	xmm0, xmm4
	movups	XMMWORD PTR _son[edx*4+16], xmm0
	movups	XMMWORD PTR _prnt[edx*4+2532], xmm1
	movd	xmm0, eax
	lea	eax, DWORD PTR [edx+12]
	pshufd	xmm1, xmm0, 0
	paddd	xmm1, xmm2
	movaps	xmm0, xmm1
	paddd	xmm0, xmm4
	movups	XMMWORD PTR _son[edx*4+32], xmm0
	movups	XMMWORD PTR _prnt[edx*4+2548], xmm1
	movd	xmm0, eax
	pshufd	xmm1, xmm0, 0
	paddd	xmm1, xmm2
	movups	XMMWORD PTR _freq[edx*4], xmm3
	movups	XMMWORD PTR _freq[edx*4+16], xmm3
	movaps	xmm0, xmm1
	movups	XMMWORD PTR _freq[edx*4+32], xmm3
	paddd	xmm0, xmm4
	movups	XMMWORD PTR _freq[edx*4+48], xmm3
	movups	XMMWORD PTR _son[edx*4+48], xmm0
	movups	XMMWORD PTR _prnt[edx*4+2564], xmm1
	add	edx, 16					; 00000010H
	cmp	edx, 304				; 00000130H
	jl	$LL4@startHuff

; 183  : 	int i, j;
; 184  : 
; 185  : 	for (i = 0; i < N_CHAR; i++) {

	cmp	edx, 315				; 0000013bH
	jge	SHORT $LN3@startHuff
	push	edi
	mov	ecx, 315				; 0000013bH
	lea	edi, DWORD PTR _freq[edx*4]
	sub	ecx, edx
	mov	eax, 1
	rep stosd
	pop	edi
	npad	9
$LL13@startHuff:

; 186  : 		freq[i] = 1;
; 187  : 		son[i] = i + LZ_T;

	lea	eax, DWORD PTR [edx+629]

; 188  : 		prnt[i + LZ_T] = i;

	mov	DWORD PTR _prnt[edx*4+2516], edx
	mov	DWORD PTR _son[edx*4], eax
	inc	edx
	cmp	edx, 315				; 0000013bH
	jl	SHORT $LL13@startHuff
$LN3@startHuff:

; 189  : 	}
; 190  : 	for (i = 0,  j = N_CHAR; j <= LZ_R; i += 2, j++) {

	xor	ecx, ecx
	mov	edx, 315				; 0000013bH
	npad	12
$LL7@startHuff:

; 191  : 		freq[j] = freq[i] + freq[i + 1];

	mov	eax, DWORD PTR _freq[ecx*4+4]
	add	eax, DWORD PTR _freq[ecx*4]

; 192  : 		son[j] = i;

	mov	DWORD PTR _son[edx*4], ecx

; 193  : 		prnt[i] = prnt[i + 1] = j;

	mov	DWORD PTR _prnt[ecx*4+4], edx
	mov	DWORD PTR _prnt[ecx*4], edx
	add	ecx, 2
	mov	DWORD PTR _freq[edx*4], eax
	inc	edx
	cmp	edx, 628				; 00000274H
	jle	SHORT $LL7@startHuff

; 194  : 	}
; 195  : 	freq[LZ_T] = 0xffff;

	mov	DWORD PTR _freq+2516, 65535		; 0000ffffH

; 196  : 	prnt[LZ_R] = 0;

	mov	DWORD PTR _prnt+2512, 0

; 197  : }

	ret	0
_startHuff ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _GetByte
_TEXT	SEGMENT
_GetByte PROC						; COMDAT

; 163  : 	unsigned i;
; 164  : 
; 165  : 	while (getlen <= 8) {

	mov	al, BYTE PTR _getlen
	cmp	al, 8
	ja	SHORT $LN10@GetByte
	push	esi
	npad	6
$LL2@GetByte:

; 166  : 		if ((i = getc(infile)) < 0) i = 0;

	push	DWORD PTR _infile
	call	_getc
	mov	esi, eax

; 167  : 		getbuf |= i << (8 - getlen);

	mov	ecx, 8
	mov	al, BYTE PTR _getlen
	add	esp, 4
	movzx	edx, al

; 168  : 		getlen += 8;

	add	al, 8
	sub	ecx, edx
	mov	BYTE PTR _getlen, al
	mov	edx, DWORD PTR _getbuf
	shl	esi, cl
	or	edx, esi
	mov	DWORD PTR _getbuf, edx
	cmp	al, 8
	jbe	SHORT $LL2@GetByte

; 163  : 	unsigned i;
; 164  : 
; 165  : 	while (getlen <= 8) {

	pop	esi
	jmp	SHORT $LN3@GetByte
$LN10@GetByte:
	mov	edx, DWORD PTR _getbuf
$LN3@GetByte:

; 169  : 	}
; 170  : 	i = getbuf;

	mov	ecx, edx

; 171  : 	getbuf <<= 8;
; 172  : 	getlen -= 8;

	add	al, 248					; 000000f8H
	shl	ecx, 8

; 173  : 	return i >> 8;

	shr	edx, 8
	mov	BYTE PTR _getlen, al
	mov	eax, edx
	mov	DWORD PTR _getbuf, ecx

; 174  : }

	ret	0
_GetByte ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _GetBit
_TEXT	SEGMENT
_GetBit	PROC						; COMDAT

; 148  : 	int i;
; 149  : 
; 150  : 	while (getlen <= 8) {

	mov	al, BYTE PTR _getlen
	cmp	al, 8
	ja	SHORT $LN10@GetBit
	push	esi
	npad	6
$LL2@GetBit:

; 151  : 		if ((i = getc(infile)) < 0) i = 0;

	push	DWORD PTR _infile
	call	_getc
	mov	esi, eax
	add	esp, 4
	xor	eax, eax

; 152  : 		getbuf |= i << (8 - getlen);

	mov	ecx, 8
	test	esi, esi
	cmovs	esi, eax
	mov	al, BYTE PTR _getlen
	movzx	edx, al

; 153  : 		getlen += 8;

	add	al, 8
	sub	ecx, edx
	mov	BYTE PTR _getlen, al
	mov	edx, DWORD PTR _getbuf
	shl	esi, cl
	or	edx, esi
	mov	DWORD PTR _getbuf, edx
	cmp	al, 8
	jbe	SHORT $LL2@GetBit

; 148  : 	int i;
; 149  : 
; 150  : 	while (getlen <= 8) {

	pop	esi
	jmp	SHORT $LN3@GetBit
$LN10@GetBit:
	mov	edx, DWORD PTR _getbuf
$LN3@GetBit:

; 154  : 	}
; 155  : 	i = getbuf;

	lea	ecx, DWORD PTR [edx+edx]

; 156  : 	getbuf <<= 1;
; 157  : 	getlen--;

	dec	al

; 158  : 	return (i < 0);

	shr	edx, 31					; 0000001fH
	mov	BYTE PTR _getlen, al
	mov	eax, edx
	mov	DWORD PTR _getbuf, ecx

; 159  : }

	ret	0
_GetBit	ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File D:\Projects\mlbr\lzhuf.c
;	COMDAT _Error
_TEXT	SEGMENT
_message$ = 8						; size = 4
_Error	PROC						; COMDAT

; 27   : 	printf("\n%s\n", message);

	push	DWORD PTR _message$[esp-4]
	push	OFFSET ??_C@_04EINJHBMM@?6?$CFs?6@
	call	_printf
	add	esp, 8

; 28   : 	exit(EXIT_FAILED);

	push	-1
	call	_exit
$LN3@Error:
	int	3
_Error	ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\Program Files (x86)\Windows Kits\10\include\10.0.18362.0\ucrt\stdio.h
;	COMDAT _printf
_TEXT	SEGMENT
__Format$ = 8						; size = 4
_printf	PROC						; COMDAT

; 954  :     {

	push	esi

; 955  :         int _Result;
; 956  :         va_list _ArgList;
; 957  :         __crt_va_start(_ArgList, _Format);
; 958  :         _Result = _vfprintf_l(stdout, _Format, NULL, _ArgList);

	mov	esi, DWORD PTR __Format$[esp]
	push	1
	call	___acrt_iob_func
	add	esp, 4

; 643  :         return __stdio_common_vfprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);

	lea	ecx, DWORD PTR __Format$[esp+4]
	push	ecx
	push	0
	push	esi
	push	eax
	call	___local_stdio_printf_options
	push	DWORD PTR [eax+4]
	push	DWORD PTR [eax]
	call	___stdio_common_vfprintf
	add	esp, 24					; 00000018H

; 959  :         __crt_va_end(_ArgList);
; 960  :         return _Result;

	pop	esi

; 961  :     }

	ret	0
_printf	ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\Program Files (x86)\Windows Kits\10\include\10.0.18362.0\ucrt\stdio.h
;	COMDAT __vfprintf_l
_TEXT	SEGMENT
__Stream$ = 8						; size = 4
__Format$ = 12						; size = 4
__Locale$ = 16						; size = 4
__ArgList$ = 20						; size = 4
__vfprintf_l PROC					; COMDAT

; 643  :         return __stdio_common_vfprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);

	push	DWORD PTR __ArgList$[esp-4]
	push	DWORD PTR __Locale$[esp]
	push	DWORD PTR __Format$[esp+4]
	push	DWORD PTR __Stream$[esp+8]
	call	___local_stdio_printf_options
	push	DWORD PTR [eax+4]
	push	DWORD PTR [eax]
	call	___stdio_common_vfprintf
	add	esp, 24					; 00000018H

; 644  :     }

	ret	0
__vfprintf_l ENDP
_TEXT	ENDS
; Function compile flags: /Ogtpy
; File C:\Program Files (x86)\Windows Kits\10\include\10.0.18362.0\ucrt\corecrt_stdio_config.h
;	COMDAT ___local_stdio_printf_options
_TEXT	SEGMENT
___local_stdio_printf_options PROC			; COMDAT

; 87   :         static unsigned __int64 _OptionsStorage;
; 88   :         return &_OptionsStorage;

	mov	eax, OFFSET ?_OptionsStorage@?1??__local_stdio_printf_options@@9@9 ; `__local_stdio_printf_options'::`2'::_OptionsStorage

; 89   :     }

	ret	0
___local_stdio_printf_options ENDP
_TEXT	ENDS
END
