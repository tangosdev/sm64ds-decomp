/* storage 0x02396CC0 runtime 0x027C8C38 size 0x30 main. */

unsigned int fn_02396CC0(void) {
    unsigned short *reg;
    unsigned int value;

    *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xA2) = 0;
    reg = (unsigned short *)0x04808038;
    value = *reg | 2;
    *reg = (unsigned short)value;
    return value;
}
