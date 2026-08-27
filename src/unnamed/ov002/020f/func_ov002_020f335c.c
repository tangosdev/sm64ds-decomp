#pragma opt_common_subs off
void func_ov002_020f335c(char *o, int i) {
    int idx = i * 0x30;
    int *fld = (int *)(o + 0x144 + idx);
    *fld = *fld + 0x1800;
    if (*fld >= 0x66000) {
        *fld = 0x66000;
        *(unsigned char *)(o + idx + 0x15c) = 0;
        *(unsigned char *)(o + idx + 0x15e) = 0;
    }
    char *b = o + idx;
    int f140 = *(int *)(b + 0x140) >> 0xc;
    int f148 = *(int *)(b + 0x148) >> 0xc;
    int f14c = *(int *)(b + 0x14c) >> 0xc;
    int fv   = *fld >> 0xc;
    *(volatile unsigned short *)0x4001042 = (unsigned short)(((f140 << 8) & 0xff00) | (f148 & 0xff));
    *(volatile unsigned short *)0x4001046 = (unsigned short)(((fv   << 8) & 0xff00) | (f14c & 0xff));
}
