/* storage 0x02396BAC runtime 0x027C8B24 size 0xC8 main. */

extern void fn_02395354(unsigned int addr, unsigned int val);
extern unsigned int fn_02395334(unsigned int addr);
extern void fn_02382C98(unsigned char *src, unsigned int dst, unsigned int count);

int fn_02396BAC(unsigned int count, unsigned char *src) {
    char *base;
    unsigned int cursor;
    unsigned int i;

    if (count > 0x80)
        return 4;
    if (count != 0) {
        base = *(char **)0x0380FFF4;
        if ((*(unsigned short *)(base + 0x300 + 0x9E) & 1) != 0) {
            cursor = *(unsigned int *)(base + 0x398);
            fn_02395354(cursor, 0xFF);
            cursor = cursor + 1;
            i = 0;
            while (i < count) {
                fn_02395354(cursor, fn_02395334((unsigned int)src));
                cursor = cursor + 1;
                src = src + 1;
                i = i + 1;
            }
        } else {
            fn_02382C98(src, *(unsigned int *)(base + 0x398), count + 1);
        }
    }
    {
        char **pp = (char **)0x0380FFF4;
        *(unsigned short *)(*pp + 0x300 + 0x9C) = (unsigned short)count;
        *(unsigned short *)(*pp + 0x300 + 0xA0) = 1;
    }
    return 0;
}
