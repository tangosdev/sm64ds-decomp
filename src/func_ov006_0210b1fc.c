void func_ov006_020c1f04(char *c, int *src);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
void func_ov006_020c1ef8(int *p);
void FreeGfxSlotsById(int arg);

void func_ov006_0210b1fc(char *p)
{
    if (*(int *)(p + 0x500c) == 0x50) {
        func_ov006_020c1f04(p + 0x4f38, 0);
        func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    }

    if (*(int *)(p + 0x500c) > 0) {
        *(int *)(((int)p + 0x500c) & 0xFFFFFFFFFFFFFFFF) -= 1;
        if (*(int *)(p + 0x500c) != 0)
            return;
        *(unsigned char *)(p + 0x503b) = *(unsigned char *)(p + 0x503c);
        func_ov006_020c1ef8((int *)(p + 0x4f38));
        return;
    }

    *(unsigned char *)(((int)p + 0x503e) & 0xFFFFFFFFFFFFFFFF) -= 1;
    if (*(unsigned char *)(p + 0x503e) != 0)
        return;

    FreeGfxSlotsById(0x1d);

    {
        int *q = (int *)(p + 0x5000);
        if (*(unsigned char *)(p + 0xc4) == 0) {
            *(unsigned char *)(p + 0xc3) = 1;
            *(unsigned char *)(p + 0xc4) = 1;
            *(short *)(p + 0xc0) = 0;
        }
        *q += 1;
    }
}
