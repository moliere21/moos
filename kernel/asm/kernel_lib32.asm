;=================================================
;符号导入
;=================================================
extern screen_cursor
extern screen_width
extern screen_high
;=================================================
;符号导出
;=================================================
global _Low_PrintStr
global _Low_PrintChar
global _Low_IntToStr
global _Low_ScreenClean
global _Low_MemCopy

;=================================================
;函数： 内存复制  MemCopy
;=================================================
;=================================================
;参数:
;       es:esi      ->      源内存地址
;       es:edi      ->      目的内存地址
;       ecx         ->      内存字节数
;返回值:
;       eax         ->      复制的内存字节数
;       int MemCopy(void* dst, void* src, int count)
;=================================================
_Low_MemCopy:
    push edi
    push esi
    push ecx

    mov ecx, [esp + 4 * 6]
    mov esi, [esp + 4 * 5]
    mov edi, [esp + 4 * 4]
.COPY_LOOP:
    cmp ecx, 0
    je  .COPY_DONE
    lodsb
    mov byte [es:edi], al
    inc edi
    loop .COPY_LOOP

.COPY_DONE:
    pop ecx
    pop esi
    pop edi
    ret

;=================================================
;变量 : 保存 存储光标位置 的地址
;=================================================

;=================================================
;字符串首地址自动压栈
;CHAR_LF:0x0A   换行字符
;CHAR_NUL:0x00   结束符
;长 x 宽 : 80 x 25
;=================================================
_Low_PrintStr:                   ;0x9052f
    xor ecx, ecx                            ;保存字符个数
    mov esi, dword [esp + 4]                 ;有esi
    mov edi, [screen_cursor]                          ;只有di:16byte

CHAR_LOOP:
    mov ah, 0x0C                            ;默认设置 ah = 0xC
    lodsb
    cmp al, 0x00
    je  CHAR_NUL
    cmp al, 0x0A
    je  CHAR_LF

    mov word [gs:edi], ax
    add edi, 2
    inc ecx
    jmp CHAR_LOOP

CHAR_LF:            ;0x90560
    add edi, 0xA0

    xor edx, edx                            ;只需要eax中的除数
    mov eax, edi
    push eax                                ;保存eax
    mov ebx, 0xA0
    div ebx

    pop eax                                 ;取出eax
    sub eax, edx                            ;减掉余数
    mov edi, eax
    inc ecx
    jmp CHAR_LOOP

CHAR_NUL:                                   ;0x9056c
    mov dword [screen_cursor], edi
    inc ecx
    mov eax, ecx                            ;返回值，字符个数
    ret

;=================================================
;函数 PrintChar
;=================================================

;=================================================
;打印字符
;参数：dx -> 存放需要打印的字符 栈中占4byte
;=================================================
_Low_PrintChar:
    mov dx, word [esp + 4]
    mov edi, dword [screen_cursor]                          ;只有di:16byte
    mov word [gs:edi], dx

    add edi, 2
    mov dword [screen_cursor], edi
    ret
;=================================================
;函数 IntToStr
;=================================================

;=================================================
;整型转字符串
;参数：eax -> 存放需要转字符串的数字
;=================================================
_Low_IntToStr:
    mov ebx, 10
    mov ecx, 0
DIV_10_LOOP:
    cmp eax, 0
    je STACK_PRT

    xor edx, edx
    div ebx

    add dl, 0x30                                ;数字转ascii
    mov dh, 0xC                                 ;字符颜色
    push edx
    inc ecx
    jmp DIV_10_LOOP

STACK_PRT:
    cmp ecx, 0
    je STACK_PRT_DONE
    dec ecx
    call _Low_PrintChar
    add esp, 4
    jmp STACK_PRT
STACK_PRT_DONE:
    ret


;=================================================
;函数 ScreenClean 变量地址
;=================================================
;screenWidth   equ   LOADER_PHY_ADDR + _screenWidth
;screenHigh    equ   LOADER_PHY_ADDR + _screenHigh
;=================================================
;清屏函数
;=================================================
_Low_ScreenClean:
    mov eax, [screen_high]
    mov ebx, [screen_width]
    mul ebx
    mov ebx, 2
    div ebx

    mov edi, 0
F_BYTE_LOOP:
    cmp eax, 0
    je CLEAN_DONE
    dec eax
    mov dword [gs:edi], 0
    add edi, 4
    jmp F_BYTE_LOOP
CLEAN_DONE:
    mov dword [screen_cursor], 0
    ret