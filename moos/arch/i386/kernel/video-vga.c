#include <vga.h>

/*   屏幕光标位置   */
int screen_cursor = (80 * 3) * 2;

/*   屏幕长宽   */
int screen_width = 80;
int screen_high = 25;


/*   输出字符串   */
asmlinkage uint32_t puts(char* string) {
    return _Low_PrintStr(string);
}


/*   输出一个字符   */
asmlinkage void putchar(char ch) {
    return _Low_PrintChar(ch);
}