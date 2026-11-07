/* storage 0x02396CF0 runtime 0x027C8C68 size 0x54 main. */

void fn_02396CF0(void) {
    char **pp;
    unsigned short *reg;

    pp = (char **)0x0380FFF4;
    *(unsigned short *)(*pp + 0x300 + 0xA2) = 1;
    if (*(unsigned short *)(*pp + 0x400 + 0x20) != 0)
        return;
    reg = (unsigned short *)0x04808038;
    *reg = (unsigned short)(*reg & ~2);
    *(unsigned short *)0x04808048 = 0;
}
