org 0x200
    jmp START

;===========================================================
;导入相关头部
;===========================================================
%include "fat12.inc"
%include "func16.inc"
%include "pm.inc"
%include "sysconfig.inc"
;===========================================================
;   GDT 全局段描述符表
;===========================================================
;  段名                             段起始         段界限          段属性
LABLE_GDT:
DESC_NULL:       Descriptor         0,              0,              0
DESC_CODE_32:    Descriptor         0,             0xfffff - 1,   DA_32 | DA_CR  | DA_LIMIT_4K
DESC_DATA_32:    Descriptor         0,             0xfffff - 1,   DA_32 | DA_DRW | DA_LIMIT_4K
DESC_VIDEO_32:   Descriptor         0xb8000,       0xfffff - 1,   DA_32 | DA_DRW                        ;默认粒度为字节

GdtLen  equ  $ - LABLE_GDT
GdtPtr  dw  GdtLen - 1                                  ;GDT的长度
        dd  LOADER_PHY_ADDR +   LABLE_GDT               ;GDT的基地址

Selec_Code32_R0     equ        DESC_CODE_32  - DESC_NULL
Selec_Data32_R0     equ        DESC_DATA_32  - DESC_NULL
Selec_Video32_R0    equ        DESC_VIDEO_32 - DESC_NULL
;===========================================================
;打印信息
;===========================================================
Msg_MemCheckFailed:         db  "Mem Check Failed",13,10,0
Msg_MemCheckCompleled:      db  "Mem Check OK",13,10,0

Msg_LoadKerenlFailed:       db  "Kernel load failed",13,10,0
Msg_LoadKerenlCompleted:    db  "kernel load completed",13,10,0
;===========================================================
START:
    mov ax, cs
	mov ds, ax
    mov es, ax
	mov ss, ax
	mov sp, LOADER_STACK_BASE

    ; 清屏 打印字符串
    ;call DispClean

    ;检查内存信息，存放到ADRS_BUF
    mov ebx, 0
    mov di, _ADRS_BUF                                       ;es:di ->指向存储的缓冲区
MEM_CHK_LOOP:
    mov eax, 0x0000e820
    mov ecx, 20                                             ;ADRS的大小
    mov edx, 0x0534d4150
    int 0x15

    push eax
    jc  MEM_CHK_FAILED

    mov eax, dword [di + 8]
    cmp eax, [_ddMemSize]
    ja  ABOVE_MEM_FOUND
    jmp NEXT_MEM_CHK_LOOP

ABOVE_MEM_FOUND:
    mov eax, dword [di + 16]
    cmp eax, 0x00000001
    je  MEM_USEFUL
    jmp NEXT_MEM_CHK_LOOP

MEM_USEFUL:
    mov eax, dword [di + 8]
    mov dword [_ddMemSize], eax

NEXT_MEM_CHK_LOOP:
    pop eax

    add di, 20                                              ;指向下一个缓冲区位置
    inc byte [_ddMemCount]
    cmp ebx, 0                                              ;ebx == 0拿完了
    je  MEM_CHK_COMPLETED
    jmp MEM_CHK_LOOP


MEM_CHK_FAILED:
    mov byte [_ddMemCount], 0                               ;检查失败
    mov si,Msg_MemCheckFailed
    call temp_print16
    jmp $

MEM_CHK_COMPLETED:                                          ;内存检查完成
    mov si, Msg_MemCheckCompleled
    call temp_print16
    jmp LOAD_KERNEL

    ;加载 kernel.img
LOAD_KERNEL:
    xor ah, ah						
	xor dl, dl												;0->A盘  1->B盘  2->C盘 
	int 0x13                                                ;    ; 软驱复位

	mov si, KERNEL_FILENAME
	mov ax, KERNEL_SEG										;kernel 保存的段地址
	mov es, ax
	mov bx,	KERNEL_OFFSET									;kernel 保存的偏移地址
    mov eax, KERNEL_LIMIT
	call ReadFileData

	cmp al,byte ERNO_SUCCESS
	je LOAD_KERNEL_SUCCESS
	jmp LOAD_KERNEL_FAILED

LOAD_KERNEL_FAILED:
	;导入 kernel.img 失败
	mov si, Msg_LoadKerenlFailed
	call temp_print16
	jmp $

LOAD_KERNEL_SUCCESS:
	mov si,Msg_LoadKerenlCompleted
	call temp_print16
	
    call KillMotor    ;0x904e7                                      ;关闭软驱马达
    
    ;加载 GDT表
    lgdt [GdtPtr]

    ;关闭中断
    cli                         ;0x904ef

    ;打开地址线A20
    in al, 92h
    or al, 0x02
    out 92h, al


    mov eax, cr0                                            ;在cr0切换为32位模式
    or eax, 1
    mov cr0, eax

    jmp dword Selec_Code32_R0:PM_START + LOADER_PHY_ADDR

;===========================================================
; 32位代码段
;===========================================================
[section .code32]
align 32
[bits 32]


PM_START:
    mov ax, Selec_Data32_R0 ;0x90500
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov sp, STACK_DOWN + 4096

    mov ax, Selec_Video32_R0
    mov gs, ax

    call ScreenClean

    push dword Msg_EnterPM      ;0x9055f
    call PrintStr               ;下：0x90520
    add esp, 4                  ;0x90525

    push dword Msg_MemFron
    call PrintStr

    mov eax, [ddMemSize]
    call IntToStr

    push dword Msg_MemTail
    call PrintStr

    call SetupPaging

    push KERNEL_PHY_ADDR        ;0x90570
    call ImportElf

    jmp dword Selec_Code32_R0:KERNEL_ENTRY

    jmp $
%include "func32.inc"
%include "bootloader.inc"
;===========================================================
; 32位数据段
;===========================================================
[section .data32]
align 32
DATA32:
_Msg_EnterPM:           db          "Enter Protected Mode,done.",10,0
_Msg_MemFron:           db          "Memory Size:",0
_Msg_MemTail:           db          " Bytes.",10,0
_Msg_SetupPaging:       db          "Set up Paging.",10,0
_curCursor:             dd          0                       ;当前光标
_screenWidth:           dd          80                      ;当前光标
_screenHigh:            dd          25                      ;当前光标
;===========================================================
;   16位实模式下数据
;===========================================================
_ddMemCount:            dd          0                       ;内存块数目
_ddMemSize:             dd          0                       ;内存大小

_ADRS:
    _ddBaseAddrLow      dd          0                       ;基地址低32bit
    _ddBaseAddrHigh     dd          0                       ;基地址高32bit
    _ddLengthLow        dd          0                       ;地址长度低32bit
    _ddLengthHigh       dd          0                       ;地址长度高32bit
    _ddType             dd          0                       ;类型

_ADRS_BUF:    times  256  db  0                   ;内存描述符缓冲区
;===========================================================
;   32位实模式下地址符号
;===========================================================
ddMemCount              equ         LOADER_PHY_ADDR + _ddMemCount
ddMemSize               equ         LOADER_PHY_ADDR + _ddMemSize

Msg_EnterPM             equ         LOADER_PHY_ADDR + _Msg_EnterPM
Msg_MemFron             equ         LOADER_PHY_ADDR + _Msg_MemFron
Msg_MemTail             equ         LOADER_PHY_ADDR + _Msg_MemTail
Msg_SetupPaging         equ         LOADER_PHY_ADDR + _Msg_SetupPaging
STACK_DOWN:   times  4096 db  0