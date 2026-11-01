/* storage 0x0239A98C runtime 0x027CC904 size 0x11C main. */

extern int fn_02380884(unsigned int key);
extern int fn_023808C0(int token);
extern void fn_0239A7A4(unsigned int index);

int fn_0239A98C(unsigned int index, unsigned int value) {
    char **pp;
    char *base;
    unsigned short *slot;
    int token;
    unsigned int mask;
    unsigned int bit;

    token = fn_02380884(0x1000000);
    if (value < 0x40) {
        bit = 1u << index;
        pp = (char **)0x0380FFF4;
        base = *pp;
        slot = (unsigned short *)(base + 0x4EC);
        *slot = (unsigned short)(*slot | bit);
        base = *pp;
        slot = (unsigned short *)(base + 0x4EE);
        *slot = (unsigned short)(*slot | bit);
        base = *pp;
        if (*(unsigned short *)(base + 0x300 + 0x14) == 1
            && *(unsigned short *)(*(int *)(base + 0x2E0) + index * 0x1C) == 0x40)
            fn_0239A7A4(index);
    } else {
        mask = ~(1u << index);
        pp = (char **)0x0380FFF4;
        base = *pp;
        slot = (unsigned short *)(base + 0x4EE);
        *slot = (unsigned short)(*slot & mask);
        base = *pp;
        if ((((int)*(unsigned short *)(base + 0x400 + 0xEA) >> index) & 1) != 0
            && *(unsigned short *)(*(int *)(base + 0x2E0) + index * 0x1C) == 0x40) {
            slot = (unsigned short *)(base + 0x4EC);
            *slot = (unsigned short)(*slot & mask);
        }
    }
    base = *(char **)0x0380FFF4;
    *(unsigned short *)(*(int *)(base + 0x2E0) + index * 0x1C) = (unsigned short)value;
    return fn_023808C0(token);
}
