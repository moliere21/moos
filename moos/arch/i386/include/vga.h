#ifndef __VGA_H_
#define __VGA_H_
#include <kernel.h>

/*   对外提供的接口   */
asmlinkage uint32_t puts(char* string);
asmlinkage void putchar(char ch);


/****************************************************************************
 *   汇编接口-打印字符串 ： kernel_lib32.asm  
 ****************************************************************************/
extern uint32_t _Low_PrintStr(void * string);
extern void _Low_PrintChar(char ch);

/*   在光标处打印一个数字   */
extern void _Low_IntToStr(uint32_t number);
/*   清屏   */
extern void _Low_ScreenClean(void);

#endif