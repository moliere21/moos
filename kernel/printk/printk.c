#include <kernel.h>
#include <printk.h>
#include <vga.h>
#include <vsprint.h>
#include <string.h>


void printk(const char *fmt, ...){

    char buf[256];
    va_list args;

    memset(buf, 0, sizeof(buf));
    va_start(args, fmt);
    vsprint(buf, fmt, args);
    va_end(args);
    puts(buf);
}