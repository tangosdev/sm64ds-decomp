int fn_02397880(unsigned int a1) {
    int *base;
    int result;
    if (a1 > 1) {
        result = 5;
    } else {
        base = *(int **)0x0380FFF4;
        *(unsigned short *)((char *)base + 0x200 + 0xF6) = (unsigned short)a1;
        result = 0;
    }
    return result;
}
