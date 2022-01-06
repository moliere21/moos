org 0x7c00
	jmp START
;===========================================================
;导入fat12相关头部
;===========================================================
%include "fat12.inc"

;===========================================================
;宏定义
;===========================================================
StackBase 		equ 	0x7c00
LOADER_SEG		equ		0x9000                  ;
LOADER_OFFSET	equ		0x200
LOADER_LIMIT	equ		0x9fc00 - LOADER_SEG

LoaderFileName:         db      "LOADER  BIN"

Msg_LoadFailed:       	db      "file not exist."

START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, StackBase

	;清屏
	mov ax, 0x0600		;
	mov bx, 0x0700		;
	mov cx, 0			; 左上角
	mov dx, 0x0184f		; 右下角 （80，50）
	int 0x10

    ; 软驱复位
    xor ah, ah						;0x7dca
	xor dl, dl															;0->A盘  1->B盘  2->C盘 
	int 0x13

	;导入 bootloader
	mov si, LoaderFileName
	mov ax, LOADER_SEG													;Loader 保存的段地址
	mov es, ax
	mov bx,	LOADER_OFFSET			;0x7dd8									;Loader 保存的偏移地址
	mov eax, LOADER_LIMIT
	call ReadFileData

	cmp al,byte ERNO_SUCCESS			;0x7dde
	je LOAD_SUCCESS
	jmp LOAD_FAILED

LOAD_SUCCESS:
	jmp LOADER_SEG:LOADER_OFFSET


LOAD_FAILED:
	;导入 bootloader 失败
	mov al, 1
	mov bl, 0x07
	mov cx, 15
	mov bp, Msg_LoadFailed
	int 10

	jmp $

times 510-($-$$) db 0
	dw 0xaa55
