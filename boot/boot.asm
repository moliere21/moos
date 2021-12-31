org 0x7c00
	jmp START
	nop

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

LoaderFileName:         db      "LOADER  BIN"

Msg_LoadFailed:       	db      "file not exist.",13,10,0
Msg_LoaderOk:	        db      "LOADER SUCCESS",13,10,0


START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, StackBase

	;清屏
	call DispClean

    ; 软驱复位
    xor ah, ah
	xor dl, dl															;0->A盘  1->B盘  2->C盘 
	int 0x13

	;导入 bootloader
	mov si, LoaderFileName
	mov ax, LOADER_SEG													;Loader 保存的段地址
	mov es, ax
	mov bx,	LOADER_OFFSET												;Loader 保存的偏移地址
	call ReadFileData

	cmp al, ERNO_SUCCESS
	je LOAD_SUCCESS
	jmp LOAD_FAILED

LOAD_SUCCESS:
	mov si,Msg_LoaderOk
	call temp_print16
	jmp $

LOAD_FAILED:
	;导入 bootloader 失败
	mov si, Msg_LoadFailed
	call temp_print16
	jmp $

times 510-($-$$) db 0
	dw 0xaa55
