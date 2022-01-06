#include <types.h>
#include <pm.h>
#include <string.h>
#include <printk.h>


static struct idt_entry idtTable[IDT_COUNT];
struct idt_ptr* idtPtr;

void idt_install(uint8_t num, uint32_t base, uint16_t selctor, uint8_t gate, uint8_t flags){
    /* The interrupt routine's base address */
    idtTable[num].base_low = (base & 0xffff);
    idtTable[num].base_high = (base >> 16) & 0xffff;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idtTable[num].selector = selctor; 
    idtTable[num].always0 = 0; 
    idtTable[num].gate_type = 0x0f & gate;
    idtTable[num].flags = 0x0f & flags;
}

void idt_init(){
    printk("IDT Init...\n");
    _Low_GetIdtPtr();
    idtPtr->limit = (sizeof (struct idt_entry) * IDT_COUNT) - 1;
    idtPtr->base = (uint32_t)&idtTable;

    printk("IDT :\n    base :%d\n    limit:%d\n",idtPtr->base, idtPtr->limit);

    /* Add any new ISRs to the IDT here using idt_install */
    
}
