extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();

typedef struct { int a, b; } S8;

extern S8 g_e0fc;
extern S8 g_e114;
extern S8 g_e10c;
extern S8 g_e104;
extern S8 g_e134, g_e140, g_e128, g_e11c;

extern S8 g_e15c[2], g_e17c[2], g_e18c[2], g_e14c[2], g_e16c[2];
extern S8 g_dcd8, g_dcf0, g_dd10, g_dce0, g_dcf8, g_dcd0, g_dd18, g_dd00, g_dce8, g_dd08;

void __sinit_ov062_0211d6fc(void)
{
    func_02017acc(&g_e0fc, 0x348);
    func_020731dc(&g_e0fc, func_02017ab4, &g_e134);
    _ZN13SharedFilePtr9ConstructEj(&g_e114, 0x349);
    func_020731dc(&g_e114, SharedFilePtr_Destruct_Anim, &g_e140);
    _ZN13SharedFilePtr9ConstructEj(&g_e10c, 0x34a);
    func_020731dc(&g_e10c, SharedFilePtr_Destruct_Anim, &g_e128);
    _ZN13SharedFilePtr9ConstructEj(&g_e104, 0x34b);
    func_020731dc(&g_e104, SharedFilePtr_Destruct_Anim, &g_e11c);

    g_e15c[0] = g_dcd8; g_e15c[1] = g_dcf0;
    g_e17c[0] = g_dd10; g_e17c[1] = g_dce0;
    g_e18c[0] = g_dcf8; g_e18c[1] = g_dcd0;
    g_e14c[0] = g_dd18; g_e14c[1] = g_dd00;
    g_e16c[0] = g_dce8; g_e16c[1] = g_dd08;
}
