extern void SetBg2Offset(int a, int b);
extern void func_ov006_020fa9a0(char *p);
extern unsigned char TOP_SCREEN_RELATED;

void func_ov006_020fa9c8(char *p, int i)
{
    int n = i * 0x14;
    int dv;
    *(int *)(p + 0x5bfc + n) += *(int *)(p + 0x5c04 + n);
    *(int *)(p + 0x5c00 + n) += *(int *)(p + 0x5c08 + n);
    if (*(int *)(p + 0x5c04 + n) <= -0x1000)
        *(int *)(p + 0x5c04 + n) += 0x40;
    if (*(int *)(p + 0x5c08 + n) >= 0x3000)
        *(int *)(p + 0x5c08 + n) += 0x100;
    dv = *(int *)(p + 0x5c00 + n) >> 12;
    SetBg2Offset(*(int *)(p + 0x5bfc + n) >> 12, dv);
    if (dv < 0x40) return;
    *(short *)(p + 0x5c0c + n) = 0x258;
    *(unsigned char *)(p + 0x5c0f + n) = 0;
    TOP_SCREEN_RELATED &= ~4;
    func_ov006_020fa9a0(p);
}
