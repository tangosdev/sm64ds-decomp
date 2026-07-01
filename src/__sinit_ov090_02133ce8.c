extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void func_02017984();

typedef struct { int a, b; } S8;

extern S8 g_a0;
extern S8 g_88;
extern S8 g_80;
extern S8 g_90;
extern S8 g_98;
extern S8 g_c0, g_d8, g_a8, g_cc, g_b4;

extern S8 g_f4[2], g_504[2], g_514[2], g_e4[2];
extern S8 g_110, g_108, g_118, g_f8, g_e8, g_e0, g_f0, g_100;

void __sinit_ov090_02133ce8(void)
{
    func_02017acc(&g_a0, 0x39f);
    func_020731dc(&g_a0, func_02017ab4, &g_c0);
    _ZN13SharedFilePtr9ConstructEj(&g_88, 0x3a0);
    func_020731dc(&g_88, func_02017984, &g_d8);
    _ZN13SharedFilePtr9ConstructEj(&g_80, 0x3a1);
    func_020731dc(&g_80, func_02017984, &g_a8);
    _ZN13SharedFilePtr9ConstructEj(&g_90, 0x3a2);
    func_020731dc(&g_90, func_02017984, &g_cc);
    _ZN13SharedFilePtr9ConstructEj(&g_98, 0x3a3);
    func_020731dc(&g_98, func_02017984, &g_b4);

    g_f4[0] = g_110; g_f4[1] = g_108;
    g_504[0] = g_118; g_504[1] = g_f8;
    g_514[0] = g_e8; g_514[1] = g_e0;
    g_e4[0] = g_f0; g_e4[1] = g_100;
}
