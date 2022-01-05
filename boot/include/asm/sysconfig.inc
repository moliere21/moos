;内核文件的名字
KERNEL_FILENAME:    db      "KERNEL  BIN"                       ;定义内核文件 kernel.img

;===============================================================
;bootloader 加载的地址
LOADER_SEG		    equ		    0x9000                  ;
LOADER_OFFSET	    equ		    0x200
LOADER_PHY_ADDR     equ         LOADER_SEG * 0x10

LOADER_STACK_BASE   equ         0x100                           ;loader 的栈地址
;===============================================================
;页目录大小:    4 * 1024 = 4K
;页表大小:      1024 * 4 * 1024 = 4M
PAGE_DIR_BASE       equ         0x100000
PAGE_TABLE_BASE     equ         0x101000                        ;页表开始地址       1M + 4K

;===============================================================
;内核加载的内存地址
KERNEL_SEG          equ         0x7000
KERNEL_OFFSET       equ         0x0                             ;内核加载的偏移地址
KERNEL_PHY_ADDR     equ         KERNEL_SEG * 0x10               ;内核加载的物理地址

;为内核预留的内存空间大小
KERNEL_LIMIT        equ         LOADER_PHY_ADDR - KERNEL_PHY_ADDR
KERNEL_ENTRY        equ         0x1000
;===============================================================
;bootloader 传递给 kernel 的启动参数存放地址
CMDLINE_ADDR        equ         0x700
CMDLINE_MAGIC       equ         0x328