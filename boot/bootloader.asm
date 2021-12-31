org 0x100
    jmp START

;===========================================================
;导入fat12相关头部
;===========================================================
%include "fat12.inc"

;===========================================================
;宏定义
;===========================================================
LoaderStackBase equ 0x100
LoaderMsg db "Loadering image..."


;===========================================================
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