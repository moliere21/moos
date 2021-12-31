;=======================
; 段相关
;=======================



temp_print16:
loop:
    lodsb   ; ds:si -> al
    or al,al
    jz done 
    mov ah,0x0e        
    mov bx,15        
    int 0x10        
    jmp loop
done:
    ret

;=======================
; 宏定义
;=======================
Msg_Boot: db "Booting..."



;=========================
;打印字符串
;arg: bp  存放字符串地址
;     ah  字符串长度
;=========================
PrintStr:
	mov al, 1
	mov bh, 0
	mov bl, 0x07
	mov cx, 10
	mov dh, 0
	mov dh, 0

	push ds
	pop es
	mov bp, Msg_Boot
	mov ah, 0x13
	int 0x10
	ret


;=========================
;清屏
;=========================
DispClean:
	mov ax, 0x0600		;
	mov bx, 0x0700		;
	mov cx, 0			; 左上角
	mov dx, 0x0184f		; 右下角 （80，50）
	int 0x10
	ret

;=========================
;清软驱
;=========================
SoftDiskReset:
	xor ah, ah
	xor dl, dl			;0->A盘  1->B盘  2->C盘 
	int 0x13
	ret

StackReset:

	ret