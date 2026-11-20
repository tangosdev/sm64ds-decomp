/* storage 0x02396D44 runtime 0x027C8CBC size 0x94 main. */

int fn_02396D44(unsigned int arg) {
    char *base;
    unsigned int quot;

    if (arg == 0xFFFF) {
        char **pp = (char **)0x0380FFF4;
        *(unsigned short *)(*pp + 0x200 + 0xFC) = 0xFF;
        *(unsigned short *)(*pp + 0x300 + 0x88) = 0xFFFF;
    } else {
        base = *(char **)0x0380FFF4;
        quot = (arg * *(unsigned short *)(base + 0x300 + 0x6A)) / 100;
        if (quot > 0x10000)
            return 5;
        *(unsigned short *)(base + 0x200 + 0xFC) = (unsigned short)arg;
        *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x88) = (unsigned short)quot;
    }
    return 0;
}
