extern int DecIfAbove0_Short(char *p);
extern int RandomIntInternal(char *p);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern void func_ov035_021118a8(char *t);
extern int func_ov035_02111798(char *c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char *c);
extern unsigned char data_0209f2c0[];
extern short data_ov035_02112b80[];
extern int data_0209e650[];
int func_ov035_0211195c(char *c) {
    unsigned char idx = data_0209f2c0[0];
    *(short*)(c+0x92) = data_ov035_02112b80[idx];
    if (idx == 2) {
        if (DecIfAbove0_Short(c+0x320) == 0) {
            int r = (unsigned short)((unsigned)RandomIntInternal((char*)data_0209e650) >> 16);
            if ((unsigned)r >= 0x7fff) *(signed char*)(c+0x31e) = 1;
            else *(signed char*)(c+0x31e) = -1;
            *(short*)(c+0x320) = (short)((r % 4 + 1) * 0x1e);
            *(unsigned short*)(c+0x322) = *(unsigned short*)(c+0x320);
        } else {
            if ((int)*(unsigned short*)(c+0x320) < (int)*(unsigned short*)(c+0x322) - 5) {
                short *q = (short*)(((int)c + 0x92) & 0xFFFFFFFFFFFFFFFF);
                *q = (short)(*q * *(signed char*)(c+0x31e));
            } else {
                *(short*)(c+0x92) = 0;
            }
        }
    }
    func_020393a4((int*)(c+0x124), 0x180000);
    func_02039394((int*)(c+0x124), 0x1000);
    {
        short *s = (short*)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFF);
        *s = (short)(*s + *(short*)(c+0x92));
    }
    func_ov035_021118a8(c);
    func_ov035_02111798(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x180000, 0x1000))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
