;===============================================================
;导入 导出
;---------------------------------------------------------------
global _start

extern moos_main
;===============================================================
;数据段
;---------------------------------------------------------------
[section .data]
align 32
[bits 32]
;_curCursor:             dd          0                       ;当前光标
;_screenWidth:           dd          80                      ;当前光标
;_screenHigh:            dd          25                      ;当前光标
;===============================================================
;未初始化堆栈段
;---------------------------------------------------------------
[section .bss]
align 32
[bits 32]
KERNEL_STACK:           resb    4*1024                          ;4kb 栈大小
STACK_OF_KERNEL:   

;===============================================================
;代码段
;---------------------------------------------------------------
[section .text]
align 32
[bits 32]

_start:
    mov ax, ds
    mov es, ax
    mov ss, ax                                                
    mov fs, ax
    mov esp, STACK_OF_KERNEL

    jmp moos_main

    ;will not be exec
    jmp $

    