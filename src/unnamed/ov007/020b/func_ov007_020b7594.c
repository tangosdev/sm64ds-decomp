extern int func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c0354(int r0, unsigned r1);
extern int func_ov007_020c1068(void);
extern int func_ov007_020c0604(int a, int b);
int func_ov007_020b7594(int r0arg, int r1arg)
{
    int* p = (int*)func_ov007_020c3df4(0, 0x48);
    p[4] = func_ov007_020c3df4(0, 0x400);
    p[5] = func_ov007_020c3df4(0, 0x400);
    func_ov007_020c0354(p[4], 0x80);
    func_ov007_020c0354(p[5], 0x80);
    p[0] = func_ov007_020c1068();
    p[1] = func_ov007_020c1068();
    p[2] = func_ov007_020c0604(1, r0arg);
    p[3] = func_ov007_020c0604(2, r1arg);
    p[6] = 0;
    p[7] = 0;
    p[8] = 0;
    p[9] = 0;
    p[10] = 0;
    p[11] = 0;
    p[14] = 0;
    p[15] = 0;
    p[16] = 0;
    p[17] = 0;
    p[12] = 0;
    p[13] = 0;
    return (int)p;
}
