extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Clsn();

typedef struct { int a, b; } S8;

extern S8 g_9a4, g_9ac, g_9b4, g_99c;
extern S8 g_9c8, g_9d4, g_9e0, g_9bc;

extern S8 g_a24[7];
extern S8 g_4b0, g_4f8, g_4c8, g_4c0, g_4a8, g_4b8, g_4a0;

extern S8 g_9ec[7];
extern S8 g_500, g_4f0, g_4e0, g_4d0, g_508, g_4d8, g_4e8;

void __sinit_ov015_02113048(void)
{
    func_02017acc(&g_9a4, 0x58d);
    func_020731dc(&g_9a4, func_02017ab4, &g_9c8);
    func_02017b4c(&g_9ac, 0x58e);
    func_020731dc(&g_9ac, SharedFilePtr_Destruct_Clsn, &g_9d4);
    func_02017acc(&g_9b4, 0x58b);
    func_020731dc(&g_9b4, func_02017ab4, &g_9e0);
    func_02017b4c(&g_99c, 0x58c);
    func_020731dc(&g_99c, SharedFilePtr_Destruct_Clsn, &g_9bc);

    g_a24[0] = g_4b0;
    g_a24[1] = g_4f8;
    g_a24[2] = g_4c8;
    g_a24[3] = g_4c0;
    g_a24[4] = g_4a8;
    g_a24[5] = g_4b8;
    g_a24[6] = g_4a0;

    g_9ec[0] = g_500;
    g_9ec[1] = g_4f0;
    g_9ec[2] = g_4e0;
    g_9ec[3] = g_4d0;
    g_9ec[4] = g_508;
    g_9ec[5] = g_4d8;
    g_9ec[6] = g_4e8;
}
