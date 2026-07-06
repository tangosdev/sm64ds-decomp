extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void func_02017984();

typedef struct { int a, b; } S8;

extern S8 g_968;
extern S8 g_978;
extern S8 g_980;
extern S8 g_970;
extern S8 g_988, g_994, g_9a0, g_9ac;

typedef struct { S8 a, b; int pad; } S20;
extern S20 g_76c[6];
extern S8 g_6e0, g_6b8, g_6f0, g_6c0, g_6a8, g_6a0, g_6f8, g_6b0, g_6d8, g_6c8, g_6e8, g_6d0;

void __sinit_ov014_02113118(void)
{
    func_02017acc(&g_968, 0x9c02);
    func_020731dc(&g_968, func_02017ab4, &g_988);
    func_02017acc(&g_978, 0x9c01);
    func_020731dc(&g_978, func_02017ab4, &g_994);
    _ZN13SharedFilePtr9ConstructEj(&g_980, 0x9c04);
    func_020731dc(&g_980, func_02017984, &g_9a0);
    _ZN13SharedFilePtr9ConstructEj(&g_970, 0x9c03);
    func_020731dc(&g_970, func_02017984, &g_9ac);

    g_76c[0].a = g_6e0; g_76c[0].b = g_6b8;
    g_76c[1].a = g_6f0; g_76c[1].b = g_6c0;
    g_76c[2].a = g_6a8; g_76c[2].b = g_6a0;
    g_76c[3].a = g_6f8; g_76c[3].b = g_6b0;
    g_76c[4].a = g_6d8; g_76c[4].b = g_6c8;
    g_76c[5].a = g_6e8; g_76c[5].b = g_6d0;
}
