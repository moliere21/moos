#ifndef __LINKEAGE_H
#define __LINKEAGE_H_


/*	fatcall : 函数参数保存在寄存器	*/
#define fastcall __attribute__((regparm(3)))

/*	asmlinkage : 函数参数压栈传递	*/
#define asmlinkage __attribute__((regparm(0)))















#endif
