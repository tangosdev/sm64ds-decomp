extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();

typedef struct { int a, b; } S8;

extern S8 g_c88, g_ca0, g_c90, g_c98;
extern S8 g_cc0, g_ccc, g_ca8, g_cb4;

extern S8 g_ce8[2], g_cf8[2], g_d08[2], g_d18[2], g_cd8[2];
extern S8 g_a20, g_a28, g_a38, g_a40, g_a18, g_a08, g_a10, g_a48, g_a30, g_a00;

void __sinit_ov077_021275fc(void)
{
    func_02017acc(&g_c88, 0x40d);
    func_020731dc(&g_c88, func_02017ab4, &g_cc0);
    _ZN13SharedFilePtr9ConstructEj(&g_ca0, 0x40e);
    func_020731dc(&g_ca0, SharedFilePtr_Destruct_Anim, &g_ccc);
    _ZN13SharedFilePtr9ConstructEj(&g_c90, 0x40f);
    func_020731dc(&g_c90, SharedFilePtr_Destruct_Anim, &g_ca8);
    _ZN13SharedFilePtr9ConstructEj(&g_c98, 0x410);
    func_020731dc(&g_c98, SharedFilePtr_Destruct_Anim, &g_cb4);

    g_ce8[0] = g_a20; g_ce8[1] = g_a28;
    g_cf8[0] = g_a38; g_cf8[1] = g_a40;
    g_d08[0] = g_a18; g_d08[1] = g_a08;
    g_d18[0] = g_a10; g_d18[1] = g_a48;
    g_cd8[0] = g_a30; g_cd8[1] = g_a00;
}
