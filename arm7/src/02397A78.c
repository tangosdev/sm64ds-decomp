#pragma opt_propagation off
int fn_02397A78(unsigned int a1) {
    int *base;
    int zero;
    if (a1 > 0xFF)
        return 5;
    zero = 0;
    base = *(int *volatile *)0x0380FFF4;
    *(unsigned short *)((char *)base + 0x300 + 0x7C) = (unsigned short)zero;
    base = *(int *volatile *)0x0380FFF4;
    *(unsigned short *)((char *)base + 0x300 + 0x7A) = (unsigned short)a1;
    return zero;
}
