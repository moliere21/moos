BS_OEMName      db    "MOOS-OEM"        ; OEM name / 8 B
BPB_BytsPerSec  dw    512               ; 一个扇区512字节 
BPB_SecPerClus  db    1                 ; 每个簇一个扇区
BPB_RsvdSecCnt  dw    1                 ; 保留扇区数, 必须为1
BPB_NumFATs     db    2                 ; FAT表份数
BPB_RootEntCnt  dw    224               ; 根目录项数
BPB_TotSec16    dw    2880              ; RolSec16, 总扇区数
BPB_Media       db    0xf0              ; 介质种类: 移动介质
BPB_FATSz16     dw    9                 ; FATSz16 分区表占用扇区数
BPB_SecPerTrk   dw    18                ; SecPerTrk, 磁盘 
BPB_NumHeads    dw    2                 ; 磁头数    
BPB_HiddSec     dd    0                 ; HiddSec
BPB_TotSec32    dd    2880              ; 卡容量
BS_DrvNum       db    0                 ; DvcNum
BS_Reserved1    db    0                 ; NT保留    
BS_BootSig      db    0x29              ; BootSig扩展引导标记
BS_VolD         dd    0xffffffff        ; VolID 
BS_VolLab       db    "MOOS "     ; 卷标
BS_FileSysType  db    "FAT12   "        ; FilesysType
;==============================================================
;读扇区函数
; es : bx  ->  读取扇区到的内存地址
; ax : 第几个扇区
; cl ; 读取几个扇区
;==============================================================
ReadSector:
	push bp				;0x7c79
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



;==============================================================
;函数   GetFatByAx  用到的变量
;==============================================================
FirstFatSector          equ         1
isOdd	                db 	        0					;FAT表项的奇偶

;==============================================================
;函数   GetFatByAx  取出固定簇号在FAT表中的值
;参数： es:bx	-> 中间扇区存放的地址  
;参数： ax : 存放簇号(表项)
;==============================================================
GetFatByAx:
	push es                                 ;保存段，和字节偏移地址
	push bx
	push ax

	mov ax, 0x9000 - 0x600
	mov es, ax
	pop ax
	
	mov byte [isOdd], 0                     ;计算出表项的整体字节数，并判断奇偶
	mov bx, 3				
	mul bx					                ;ax * 3     -->  值存放在dx ，ax
	mov bx, 2				
	div bx					                ;dx、ax / 2	-->   商放在ax、余数放在dx

	cmp dx, 0				                ;根据余数判断奇偶
	je EVEN
	mov byte [isOdd], 1		                ;是奇数

EVEN:			
	xor dx, dx                              ;只需要 商ax
	mov bx, [BPB_BytsPerSec]
	div bx                                  ;通过 dx,ax中存放的字节总数(表项 * 3 / 2) / 扇区字节数目(512)
	
	push dx                                 ;保存字节余数

	xor bx, bx
	mov bx, FirstFatSector
	add ax, bx                              ;此时 ax 中存放的第几个扇区
	mov cl, 2
	call ReadSector                         ;读两个扇区，放到 es:bx处

	pop dx                                  ;恢复余数

	cmp byte [isOdd], 1		                ;根据 isOdd 判断奇、偶
	jne EVEN_2

	add bx, dx                              ;奇数的情况-->奇数 * 3 / 2 的商自动取到第二个字节
	mov ax, word [es:bx]
	shr ax, 4
	and ax, 0x0fff
	jmp GET_FAT_OK
	
EVEN_2:	                                    ;偶数的情况-->偶数 *3 / 2 的商自动取到第一个字节
	add bx, dx
	mov ax, word [es:bx]
	and ax, 0x0fff

GET_FAT_OK:
	pop bx
	pop es
	ret

;======================================================
;函数    ReadFileData   用到的变量
;======================================================
FirstRootDirSector      equ     19
RootDirSectors          equ     14
RootDirNameLen          equ     11
DirCountPerSector       equ     16              ;一个扇区 512/32 = 16个目录项
FixedFirstDataSector    equ     31

Msg_LOADING:			db		"Loading"

findSector:	            dw      0
loopNameLen:            dw      RootDirNameLen                      ;文件名长度为11
loopSector:             db      RootDirSectors
fileSizeLimit:			dd		0

ERNO_SUCCESS:           db       0x00
ERNO_FAILED:            db      -1
;======================================================
;函数：读取文件的data 到 es:bx 地址
;参数：es:bx	-> 文件数据读取到的地址  
;参数：si 		-> 存储文件名字符串首地址 < 8 + 3 = 11><bit>
;参数: eax		-> 文件最大大小
;返回值：ax   成功保存0  失败保存-1
;======================================================
ReadFileData:
	mov word [findSector], FirstRootDirSector
	mov [fileSizeLimit], eax
SECTOR_LOOP:                                    ;第一阶段   -->   在根目录区寻找文件
	cmp byte [loopSector], 0                    ;循环 RootDirSectors  14 次数。结束了，需要将它恢复为16
	jz LOAD_FILED
	dec byte [loopSector]

	mov ax, word [findSector]
	mov cl, 1					;0x7d18
	call ReadSector                             ;读到  es:bx 处

	mov di, bx              ;0x7d1d                    ;di 指到扇区起始，即扇区第一个目录项的名字段

    push si                                     ;保存这个si 首地址
    push di                                     ;保存这个目录项的地址
	cld											;cld ：向右比较  ，std	：向左比较

	mov dx, DirCountPerSector					;一个扇区的目录项个数 16,循环16次
DIR_LOOP:
	cmp dx, 0
	jz NEXT_SECTOR_LOOP
	dec dx

	mov cx, word [loopNameLen]                                  ;文件名字长度为 11
CMP_NAME_LOOP:
	cmp cx, 0
	jz FILENAME_FOUND                           ;找到文件了-->开始读取文件的数据
	dec cx

	lodsb
	cmp al, byte [es:di]
	je SAME_BYTE
	jmp DIFFERENT_BYTE

SAME_BYTE:
	inc di
	jmp CMP_NAME_LOOP

DIFFERENT_BYTE:
    pop di                                      ;得到这个目录项首地址
    pop si                                      ;得到这个文件名地址

	add di, 32			                        ;指向下一个目录项
	
    push si
	push di				
	jmp DIR_LOOP		                        ;进行下一轮目录项比较

NEXT_SECTOR_LOOP:
	add word [findSector], 1	                ;准备开始读下一个扇区

    pop di
    pop si
	jmp SECTOR_LOOP

FILENAME_FOUND:                                 ;第二阶段   -->     导入文件data
	
	pop di                                      ;1-取出目录项地址，拿到首簇号 地址为 0x7cbe断点
    pop si

	push di
	add di, 0x1a			                    ;是首簇号 = 目录项地址 + 0x1a
	mov cx, 0
	mov cx, [es:di]	                            ;获取到了首簇号里的内容 <即下一个簇号>
	pop di
	push cx

	add di, 0x1c								;得到文件大小
	mov eax, 0
	mov eax, [es:di]
	cmp eax, [fileSizeLimit]
	ja  LOAD_FILED

	mov ax, FixedFirstDataSector	            ;Fixed过后的数据区首扇区
	add ax, cx                                  ;得到数据扇区号存放在ax

LOADING_FILE:
	mov cl, 1				;0x7d68
	call ReadSector                             ;读ax存放的扇区

	pop ax		            ;0x7d6d                   
	call GetFatByAx			                    ;取出下一个簇号	，如果大于0xff8则读完了

	cmp ax, 0xff8			;0x7d71
	jae	LOAD_COMPLETED		;above-> 大于

    cmp ax, 0xff7
    je  LOAD_FILED

	push ax				                        ;还有扇区未读，保存簇号
	mov cx, FixedFirstDataSector
	add ax, cx

	add bx, [BPB_BytsPerSec]                    ;自动增加 一个扇区的内存地址
	jc FILE_ABOVE_64K
	jmp NEXT_LOADING

FILE_ABOVE_64K:
	push ax
	mov ax, es
	add ax, 0x1000								;64K
	mov es, ax
	pop ax

NEXT_LOADING:
	jmp LOADING_FILE


LOAD_FILED:
	mov al, ERNO_FAILED
	ret

LOAD_COMPLETED:
    mov al, ERNO_SUCCESS
	ret