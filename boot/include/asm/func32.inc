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
MemCopy:
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
;函数： 导入elf格式文件到内存  ImportElf
;=================================================

;=================================================
;         int ImportElf(void* src)
;参数:
;       esp + 4 : elf文件在内存的首地址
;返回值:
;       eax
;=================================================
ImportElf:
    xor ecx, ecx
    mov esi, dword [esp + 4]
    add esi, 44                                   
    mov cx, word [esi]                          ; 拿到有多个段: 5

    sub esi, 16                                     ; 44 - 28 = 16 
    mov esi, [esi]                               ;拿到段表首地址偏移
    add esi, dword [esp + 4]                     ;拿到段表首地址 : 34
.LOAD_SEG:
    cmp ecx, 0
    je .LOAD_DONE

    mov eax, [esi]                               ;拿到 段类型
    cmp eax, 0
    je .DROP_SEG

    push dword [esi + 16]                        ;压入 复制多少字节
    mov eax, [esi + 16]

    
    mov eax, [esi + 4]
    add eax, [esp + 8]              
    push eax                                    ;段在内存的偏移

    push dword [esi + 8]
    mov eax, [esi + 8]
    call MemCopy
    add esp, 4 * 3

.DROP_SEG:
    add esi, 32                                     ;指向下一个段头
    dec ecx
    jmp .LOAD_SEG

.LOAD_DONE:
    ret
;=================================================
;变量 : 保存 存储光标位置 的地址
;=================================================
curCursor               equ         LOADER_PHY_ADDR + _curCursor
;=================================================
;字符串首地址自动压栈
;CHAR_LF:0x0A   换行字符
;CHAR_NUL:0x00   结束符
;长 x 宽 : 80 x 25
;=================================================
PrintStr:                   ;0x9052f
    xor ecx, ecx                            ;保存字符个数
    mov esi, dword [esp + 4]                 ;有esi
    mov edi, [curCursor]                          ;只有di:16byte

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
    mov dword [curCursor], edi
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
PrintChar:
    mov dx, word [esp + 4]
    mov edi, dword [curCursor]                          ;只有di:16byte
    mov word [gs:edi], dx

    add edi, 2
    mov dword [curCursor], edi
    ret
;=================================================
;函数 IntToStr
;=================================================

;=================================================
;整型转字符串
;参数：eax -> 存放需要转字符串的数字
;=================================================
IntToStr:
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
    call PrintChar
    add esp, 4
    jmp STACK_PRT
STACK_PRT_DONE:
    ret


;=================================================
;函数 ScreenClean 变量地址
;=================================================
screenWidth   equ   LOADER_PHY_ADDR + _screenWidth
screenHigh    equ   LOADER_PHY_ADDR + _screenHigh
;=================================================
;清屏函数
;=================================================
ScreenClean:
    mov eax, [screenHigh]
    mov ebx, [screenWidth]
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
    mov dword [curCursor], 0
    ret

