extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void func_02017984();

typedef struct { int a, b; } S8;

extern S8 g_d38, g_d20, g_d30, g_d28;
extern S8 g_d58, g_d64, g_d70, g_d40;

extern S8 g_d8c[2], g_d9c[2], g_dac[2], g_dbc[2], g_d7c[2];
extern S8 g_148a0, g_148a8, g_148b8, g_148c0, g_14888, g_14880, g_148b0, g_14890, g_14898, g_14878;

void __sinit_ov016_021136ec(void)
{
    func_02017acc(&g_d38, 0x3b5);
    func_020731dc(&g_d38, func_02017ab4, &g_d58);
    _ZN13SharedFilePtr9ConstructEj(&g_d20, 0x3b6);
    func_020731dc(&g_d20, func_02017984, &g_d64);
    _ZN13SharedFilePtr9ConstructEj(&g_d30, 0x3b7);
    func_020731dc(&g_d30, func_02017984, &g_d70);
    _ZN13SharedFilePtr9ConstructEj(&g_d28, 0x3b8);
    func_020731dc(&g_d28, func_02017984, &g_d40);

    g_d8c[0] = g_148a0; g_d8c[1] = g_148a8;
    g_d9c[0] = g_148b8; g_d9c[1] = g_148c0;
    g_dac[0] = g_14888; g_dac[1] = g_14880;
    g_dbc[0] = g_148b0; g_dbc[1] = g_14890;
    g_d7c[0] = g_14898; g_d7c[1] = g_14878;
}
