/* storage 0x02396F44 runtime 0x027C8EBC size 0xA4 main. */

extern void fn_023979B0(unsigned int a, unsigned int b);

int fn_02396F44(unsigned int arg) {
    char **pp;
    char *base;
    unsigned short *slot;
    unsigned short *reg;
    unsigned int bit;

    pp = (char **)0x0380FFF4;
    base = *pp;
    slot = (unsigned short *)(base + 0x316);
    bit = (unsigned int)(arg << 16);
    bit >>= 16;
    *slot = (unsigned short)((*slot & ~1u) | (bit & 1));
    if (arg != 0 && *(unsigned short *)(*pp + 0x200 + 0xF2) != 1) {
        reg = (unsigned short *)0x04808006;
        *reg = (unsigned short)(*reg | 0x40);
    } else {
        reg = (unsigned short *)0x04808006;
        *reg = (unsigned short)(*reg & ~0x40);
        fn_023979B0(*(unsigned short *)(*(char **)0x0380FFF4 + 0x300), 0);
    }
    return 0;
}
