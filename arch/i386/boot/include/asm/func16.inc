;=================================================
;打印函数
;参数：si -> 字符串首地址
;=================================================
temp_print16:
	push ax
	push bx
loop:
    lodsb   ; ds:si -> al
    or al,al
    jz done 
    mov ah,0x0e        
    mov bl,15        
    int 0x10        
    jmp loop
done:
	pop bx
	pop ax
    ret
;=================================================
;换行函数
;=================================================
NewLine:
	push ax
	push dx

	mov ah, 2h
	mov dl, 13
	int 21h
	mov dl, 10
	int 21h

	pop dx
	pop ax
	ret

;=================================================
;清屏
;=================================================
DispClean:
	mov ax, 0x0600		;
	mov bx, 0x0700		;
	mov cx, 0			; 左上角
	mov dx, 0x0184f		; 右下角 （80，50）
	int 0x10
	ret

;=================================================
;打点函数
; es : bx  ->  读取扇区到的内存地址
; ax : 第几个扇区
; cl ; 读取几个扇区
;=================================================
PrintDot:
	push ax
	push bx

	mov ah, 0xe
	mov al, '.'
	mov bl, 0xf
	int 0x10

	pop bx
	pop ax
	ret

;=================================================
;关闭软盘马达
; es : bx  ->  读取扇区到的内存地址
; ax : 第几个扇区
; cl ; 读取几个扇区
;=================================================
KillMotor:
	push dx
	mov dx, 03F2h
	mov al, 0
	out dx, al
	pop dx
	ret

