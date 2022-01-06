#ifndef __VSPRINT_H
#define __VSPRINT_H

typedef char* va_list;

#define _INTSIZEOF(n) ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
/* (sizeof(n) + sizeof(int) - 1)%sizeof(int) * sizeof(int) */

#define va_start(ap, v)  (ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap) (ap = (char*) 0)

void vsprint(char *buf, const char *fmt, va_list args);

#endif