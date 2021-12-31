org 0x7c00

	jmp START
	nop
;========================
;导入fat12相关头部
;========================
%include "fat12.inc"

;========================
;导入基本函数及变量
;========================


START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, StackBase

	;清屏
	mov ax, 0x0600		;
	mov bx, 0x0700		;
	mov cx, 0			; 左上角
	mov dx, 0x0184f		; 右下角 （80，50）
	int 0x10

    ; 软驱复位
    xor ah, ah
	xor dl, dl			;0->A盘  1->B盘  2->C盘 
	int 0x13

    mov si, Msg_Boot
    call temp_print16

    ; 开始读取 bootloader
	mov word [wSector], FirstRootDirSector		;根目录第一个扇区
SERCH_LOADER:
	cmp word [wSectorsCount], 0
	jz FILENAME_NOT_FOUND
	dec word [wSectorsCount]

	;读取一个扇区
	mov ax, LOADER_SEG
	mov es, ax
	mov bx, LOADER_OFFSET

	mov ax, [wSector]
	mov cl, 1
	call ReadSector

	mov si, LoaderFileName
	mov di, LOADER_OFFSET
	cld							;cld向右比较   std向后增长比较

	;进行一个扇区的文件名字比较 512 / 32 = 16
	mov dx, 16
SERCH_FILE:
	cmp dx, 0
	jz NEXT_SERCH_LOADER
	dec dx

	mov cx, 11
CMP_FILENAME:
	cmp cx, 0
	jz FILENAME_FOUND
	dec cx
	lodsb
	cmp al, byte [es:di]
	je GO_ON
	jmp DIFFERENT

GO_ON:
	inc di
	jmp CMP_FILENAME


DIFFERENT:
	pop si				;临时pop存储一下
	and di, 0xfff0		
	add di, 32			;指向下一个目录项
	mov si, LoaderFileName
	push di				; 保存这个目录项的地址
	jmp SERCH_FILE		;进行下一个目录项比较

NEXT_SERCH_LOADER:
	add word [wSector], 1	;准备开始读下一个扇区
	jmp SERCH_LOADER

FILENAME_NOT_FOUND:
	mov si, Msg_NotFoundLoader
	call temp_print16
	jmp $

FILENAME_FOUND:
	;1-取出目录项地址，拿到首簇号 地址为 0x7cbe断点
	pop di
	add di, 0x1a			;是首簇号
	mov cx, di
	mov cx, 0
	mov cx, [es:di]	;获取到了首簇号	;读出来的是0b？
	push cx

	; 2-计算簇号对应的扇区
	mov ax, FixedFirstDataSector	;0x7ccb
	add cx, ax

	mov ax, LOADER_SEG
	mov es, ax
	mov bx, LOADER_OFFSET
	mov ax, cx

LOADING_FILE:
	;
	push ax
	push bx

	mov ah, 0xe
	mov al, '.'
	mov bl, 0xf
	int 0x10

	pop bx
	pop ax

	;3-读扇区
	mov cl, 1
	call ReadSector


	;4-取出簇号，下一个簇号已经保存在ax
	pop ax		;取出 簇号		0x7ceb
	call GetFatByAx			;0x7cef

	cmp ax, 0xff8
	jae	LOAD_SUC		;above-> 大于


	;5- 未拿取数据完整;计算下一个扇区位子
	push ax				;保存簇号
	mov cx, FixedFirstDataSector
	add ax, cx

	add bx, [BPB_BytsPerSec]
	jmp LOADING_FILE

LOAD_SUC:
	mov si, Msg_LoaderOk
	call temp_print16

	jmp $



;=========================
;宏定义
;=========================
StackBase equ 0x7c00

LOADER_SEG		equ		0x9000
LOADER_OFFSET	equ		0x100

;==========================
;变量
;==========================
wSector	dw  0
wSectorsCount dw RootDirSectors
isOdd	db 	0					;FAT表项的奇偶


LoaderFileName db "LOADER  BIN"

Msg_Boot: db "Loadering...",13,10,0
Msg_NotFoundLoader: db "loader not Found.",13,10,0
Msg_LoaderOk:	db "LOADER SUCCESS ^_^",13,10,0

;=================================================
;打印函数
;=================================================
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
	call NextLine
    ret

;=================================================
;换行函数
;=================================================
NextLine:
	mov ah, 2h
	mov dl, 13
	int 21h
	mov dl, 10
	int 21h
	ret



;=================================================
;读扇区函数
; es : bx  ->  读取扇区到的内存地址
; ax : 第几个扇区
; cl ; 读取几个扇区
;=================================================
ReadSector:
	push bp
	mov bp, sp
	sub esp, 2

	mov byte [bp-2], cl
	push bx
	mov bl, [BPB_SecPerTrk]
	div bl
	inc ah
	mov cl, ah
	mov dh, al
	shr al, 1
	mov ch, al
	and dh, 1
	pop bx
	mov dl, [BS_DrvNum]
.GoOnReading:
	mov ah, 2
	mov al, byte [bp-2]
	int 13h
	jc .GoOnReading

	add esp, 2
	pop bp

	ret



;=============================
;读取第 ax 个 FAT表项的值
;我们要首先通过 ReadSector 读取扇区的数据
;存放到 es:bx 处
;=============================
GetFatByAx:
	push es
	push bx
	push ax

	mov ax, LOADER_SEG - 0x400
	mov es, ax
	pop ax


	;计算出表项的奇偶
	mov byte [isOdd], 0
	mov bx, 3				;bx = 3
	mul bx					;ax * bx -->值存放在dx ax
	mov bx, 2				;bx = 2
	div bx					;dx ax / 2	--> ax存放商、dx存放余数

	cmp dx, 0				;0x7db7
	je EVEN
	mov byte [isOdd], 1		;是奇数

EVEN:			;偶数
	xor dx, dx
	mov bx, [BPB_BytsPerSec]
	div bx

	;此时 dx存放这个表项的首字节，ax存放第几个扇区
	push dx
	push bx     ;保存偏移地址

	xor bx, bx
	mov bx, FirstFat
	add ax, bx
	mov cl, 2
	pop bx		;恢复偏移
	call ReadSector
	pop dx

	;dx存放的首字节


	cmp byte [isOdd], 1		;0x7dd7
	jne EVEN_2

	;奇数的情况-->奇数 * 3 / 2 的商自动取到第二个字节
	add bx, dx
	mov ax, word [es:bx]
	shr ax, 4
	and ax, 0x0fff
	jmp GET_FAT_OK


	;偶数的情况-->偶数 *3 / 2 的商自动取到第一个字节
EVEN_2:	
	;取出第一个字节存放在ax
	add bx, dx
	mov ax, word [es:bx]
	and ax, 0x0fff

GET_FAT_OK:
	pop bx
	pop es
	ret




times 510-($-$$) db 0
	dw 0xaa55
