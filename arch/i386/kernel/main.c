#include <printk.h>
#include <pm.h>




/*
 *  arch_main
 */
void arch_main(void)
{
    gdt_init();
    idt_init();
}