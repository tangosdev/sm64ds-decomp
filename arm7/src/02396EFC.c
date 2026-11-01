/* storage 0x02396EFC runtime 0x027C8E74 size 0x48 main. */

int fn_02396EFC(unsigned int arg) {
    unsigned short *slot;
    unsigned int bit;

    slot = (unsigned short *)(*(char **)0x0380FFF4 + 0x316);
    bit = (unsigned int)(arg << 15);
    bit >>= 16;
    *slot = (unsigned short)((*slot & ~2u) | ((bit & 1) << 1));
    *(unsigned short *)0x0480803C = (unsigned short)arg;
    return 0;
}
