/* gdt.c 
 * This file is modified form Bram's Kernel Development Tutorial
 * set the new gdt, the new gdt table has 256 entrys
 */
// std
#include <types.h>
// x86
#include <pm.h>
// libs
#include <string.h>
#include <printk.h>

struct tss_entry tss;

struct gdt_entry gdtTable[GDT_COUNT];  // we hava 256 gdt entry
struct gdt_ptr* gdtPtr;  // 0 - 15 : limit   16 - 47 : GDT表的base

extern void _Low_MemCopy(void* dest, void* src, uint32_t size);

/*   移动 bootloader 初始化的GDT表到内核中   */
void load_gdt(void)
{
    _Low_MemCopy(
        gdtTable,
        (void*)gdtPtr->base,
        (uint32_t)(gdtPtr->limit + 1)
    );
}


void tss_install(){
   // __asm__ volatile("ltr %%ax" : : "a"((SEG_TSS << 3)));
}

void gdt_install(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){

    /* Setup the descriptor base address */
    gdtTable[num].base_low = (base & 0xffff);
    gdtTable[num].base_middle = (base >> 16) & 0xff;
    gdtTable[num].base_high = (base >> 24) & 0xff;

    /* Setup the descriptor limits */
    gdtTable[num].limit_low = (limit & 0xffff);
    gdtTable[num].limit_high = ((limit >> 16) & 0x0f);

    /* Finally, set up the granularity and access flags */
    gdtTable[num].flags = flags;

    gdtTable[num].access = access;
}

void tss_init(){

}

void gdt_init(){

    printk("GDT Init...\n");

    /*   获取在 bootloader 中gdt的 gdtptr   */
    _Low_GetGdtPtr();

    /*   开始导入 gdt 到内核 gdtTable  */
    load_gdt();

    gdtPtr->limit = (sizeof(struct gdt_entry) * GDT_COUNT) - 1;
    gdtPtr->base = (uint32_t)&gdtTable;

    printk("GDT :\n    base :%d\n    limit:%d\n",gdtPtr->base, gdtPtr->limit);

    /*gdt_install(0, 0, 0, 0, 0);  

    gdt_install( SEG_CODE_SYS, 0, 0xfffff, DA_CR | DA_DPL0, DA_32|DA_LIMIT_4K);
    gdt_install( SEG_DATA_SYS, 0, 0xfffff, DA_DRW| DA_DPL0, DA_32|DA_LIMIT_4K); 
    gdt_install(SEG_CODE_USER, 0, 0xfffff, DA_CR | DA_DPL3, DA_32|DA_LIMIT_4K); 
    gdt_install(SEG_DATA_USER, 0, 0xfffff, DA_DRW| DA_DPL3, DA_32|DA_LIMIT_4K); 

    gdt_install(SEG_DATA_VIDEO, 0xb8000, 0xfffff, DA_DRW , DA_32); 
    */

    tss_init();

    tss_install();
}

void tss_set(uint16_t ss0, uint32_t esp0){
    memset((void *)&tss, 0, sizeof(tss));
    tss.ss0 = ss0;
    tss.esp0 = esp0;
    tss.iopb_off = sizeof(tss);
}
