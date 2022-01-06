;=================================================
;开启分页机制
;=================================================
SetupPaging:
    xor edx, edx
    mov eax, [ddMemSize]
    mov ebx, 0x400000                               ;4M大小->一个页目录对应4M
    div ebx
    
    mov ecx, eax
    test edx, edx
    jz .no_remainder
    inc ecx

.no_remainder:
    push ecx

    ;初始化页目录表
    mov ax, Selec_Data32_R0
    mov es, ax
    mov edi, PAGE_DIR_BASE
    xor eax, eax
    mov eax, PAGE_TABLE_BASE | PG_P | PG_USU | PG_RWW

.SetupPDE
    stosd
    add eax, 4096
    loop .SetupPDE

    ;初始化页表
    pop eax
    mov ebx, 1024
    mul ebx
    mov ecx, eax
    mov edi, PAGE_TABLE_BASE
    xor eax, eax
    mov eax, PG_P | PG_USU | PG_RWW

.SetupPTE:
    stosd
    add eax, 4096
    loop .SetupPTE

    ;设置 cr3 和 cr0开启分页
    mov eax, PAGE_DIR_BASE
    mov cr3, eax
    mov eax, cr0

    or eax, 0x80000000
    mov cr0, eax
    
    nop
    nop
    push Msg_SetupPaging
    call PrintStr
    add esp, 4
    ret