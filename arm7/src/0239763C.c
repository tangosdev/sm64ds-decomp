extern void fn_023969D8(int a1, int a2);

int fn_0239763C(    unsigned short *arg) {
    int *base;
    unsigned int v;
    base = *(int **)0x0380FFF4;
    fn_023969D8((int)((char *)base + 0x360), (int)arg);
    fn_023969D8(0x04808020, (int)arg);
    if ((*arg & 1) != 0) {
        v = *(volatile unsigned short *)0x048080D0;
        v &= ~0x400u;
        *(volatile unsigned short *)0x048080D0 = v;
    } else {
        v = *(volatile unsigned short *)0x048080D0;
        v |= 0x400u;
        *(volatile unsigned short *)0x048080D0 = v;
    }
    return 0;
}
