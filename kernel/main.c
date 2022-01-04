
int screen_cursor = (80 * 3) * 2;
int screen_width = 80;
int screen_high = 25;

extern void _Low_PrintStr(void * string);

/*
 * moos 主函数
 */
void moos_main(void)
{
    _Low_PrintStr("kernel starting...\n");

    while(1){};
}