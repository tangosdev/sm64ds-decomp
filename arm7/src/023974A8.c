extern void fn_02396D44(int a1);

int fn_023974A8(unsigned short a1) {
    int *base;
    if (a1 < 0xA || a1 > 0x3E8)
        return 5;
    base = *(int *volatile *)0x0380FFF4;
    *(unsigned short *)((char *)base + 0x300 + 0x6A) = a1;
    *(volatile unsigned short *)0x0480808C = a1;
    base = *(int *volatile *)0x0380FFF4;
    fn_02396D44(*(unsigned short *)((char *)base + 0x200 + 0xFC));
    return 0;
}
