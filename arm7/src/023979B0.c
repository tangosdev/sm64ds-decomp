/* storage 0x023979B0 runtime 0x027C9928 linked 0xC8 main ARM. */

extern void fn_02395354(unsigned int addr, unsigned int val);

int fn_023979B0(unsigned short a1, int a2) {
    int *base;
    int inner;
    int mid;
    unsigned short idx;
    int slot;
    unsigned short flag;
    unsigned int bit;

    if (a1 < 0xA)
        return 5;
    base = *(int **)0x0380FFF4;
    *(unsigned short *)((char *)base + 0x300) = a1;
    if (a2 != 0)
        *(unsigned short *)0x04808134 = a1;
    base = *(int **)0x0380FFF4;
    if (*(unsigned short *)((char *)base + 0x45C) != 0) {
        inner = *(int *)((char *)base + 0x464);
        mid = inner + 0x24;
        idx = *(unsigned short *)((char *)base + 0x392);
        slot = mid + idx + 6;
        flag = *(unsigned short *)((char *)base + 0x316);
        bit = (unsigned int)((int)flag << 31) >> 31;
        if (bit == 1) {
            fn_02395354(slot, a1 & 0xFF);
            fn_02395354(slot + 1, ((int)a1 >> 8) & 0xFF);
        } else {
            fn_02395354(slot, 0xFF);
            fn_02395354(slot + 1, 0xFF);
        }
    }
    return 0;
}
