org 0x100
    jmp START


%include "basefunc.inc"
LoaderStackBase equ 0x100

START:
    mov ax, cs
	mov ds, ax
    mov es, ax
	mov ss, ax
	mov sp, LoaderStackBase

    ; 清屏 打印字符串
    call DispClean
    ;call PrintStr

    mov si, LoaderMsg
    call temp_print16

    jmp $







LoaderMsg db "Loadering image..."

Msg_Boot_Len equ  18
