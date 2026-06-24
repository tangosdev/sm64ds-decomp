extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void func_02017984();

typedef struct { int a, b; } S8;

extern S8 g_ae0;
extern S8 g_af8;
extern S8 g_ae8;
extern S8 g_af0;
extern S8 g_b18, g_b24, g_b00, g_b0c;

extern S8 g_b40[2], g_b50[2], g_b60[2], g_b70[2], g_b30[2];
extern S8 g_369e0, g_369c8, g_369f8, g_36a00, g_369d8, g_369c0, g_369d0, g_36a08, g_369f0, g_369e8;

void __sinit_ov094_021367e8(void)
{
    func_02017acc(&g_ae0, 0x3cb);
    func_020731dc(&g_ae0, func_02017ab4, &g_b18);
    _ZN13SharedFilePtr9ConstructEj(&g_af8, 0x3cc);
    func_020731dc(&g_af8, func_02017984, &g_b24);
    _ZN13SharedFilePtr9ConstructEj(&g_ae8, 0x3cd);
    func_020731dc(&g_ae8, func_02017984, &g_b00);
    _ZN13SharedFilePtr9ConstructEj(&g_af0, 0x3ce);
    func_020731dc(&g_af0, func_02017984, &g_b0c);

    g_b40[0] = g_369e0; g_b40[1] = g_369c8;
    g_b50[0] = g_369f8; g_b50[1] = g_36a00;
    g_b60[0] = g_369d8; g_b60[1] = g_369c0;
    g_b70[0] = g_369d0; g_b70[1] = g_36a08;
    g_b30[0] = g_369f0; g_b30[1] = g_369e8;
}
