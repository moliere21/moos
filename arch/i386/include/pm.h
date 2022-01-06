#ifndef __PM_H 
#define __PM_H 

#include <types.h>

/*   选择子 相关   */

#define SA_RPL0			0	; ┓
#define SA_RPL1			1	; ┣ RPL
#define SA_RPL2			2	; ┃
#define SA_RPL3			3	; ┛

#define SA_TIG			0	; ┓TI
#define SA_TIL			4	; ┛

/*   属性相关   */

#define DA_DPL0			  0x00	// DPL = 0
#define DA_DPL1			  0x20	// DPL = 1
#define DA_DPL2			  0x40	// DPL = 2
#define DA_DPL3			  0x60	// DPL = 3

#define DA_DR			0x90	// 存在的只读数据段类型值
#define DA_DRW			0x92	// 存在的可读写数据段属性值
#define DA_DRWA			0x93	// 存在的已访问可读写数据段类型值
#define DA_C			0x98	// 存在的只执行代码段属性值
#define DA_CR			0x9a	// 存在的可执行可读代码段属性值
#define DA_CCO			0x9c	// 存在的只执行一致代码段属性值
#define DA_CCOR			0x9e	// 存在的可执行可读一致代码段属性值

// 系统段描述符类型
#define DA_LDT			  0x82	// 局部描述符表段类型值
#define DA_TaskGate		  0x85	// 任务门类型值
#define DA_386TSS		  0x89	// 可用 386 任务状态段类型值
#define DA_386CGATE		  0x8c	// 386 调用门类型值
#define DA_386IGATE		  0x8e	// 386 中断门类型值
#define DA_386TGATE	      0x8f	// 386 陷阱门类型值

// 描述符类型
#define DA_32			0x4	// 32 位段
#define DA_LIMIT_4K		0x8	// 段界限粒度为 4K 字节



/*   GDT 相关   */
#define GDT_COUNT 256

#define SEG_CODE_SYS    0x1
#define SEG_DATA_SYS    0x2
#define SEG_CODE_USER   0x3
#define SEG_DATA_USER   0x4
#define SEG_DATA_VIDEO  0x5
#define SEG_TSS         0x6

struct gdt_entry{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    unsigned limit_high: 4;
    unsigned flags: 4;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));




/*   IDT 相关   */
#define IDT_COUNT   256
struct idt_entry{
    uint16_t base_low;
    uint16_t selector;
    uint8_t always0;
    unsigned gate_type: 4;   
    unsigned flags: 4;  
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));


/*   TSS 相关   */
struct tss_entry{
    uint32_t link;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldtr;
    uint16_t padding1;
    uint16_t iopb_off;
} __attribute__ ((packed));



/*   接口   */
void gdt_init();
void tss_set(uint16_t ss0, uint32_t esp0);

/* kern/idt.c */
void idt_init();
void idt_install(uint8_t num, uint32_t base, uint16_t selector, uint8_t gate, uint8_t flags);

extern void _Low_GetGdtPtr();    // extern func in loader.asm
extern void _Low_GetIdtPtr();

#endif
