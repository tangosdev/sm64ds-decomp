extern int _Z14ApproachLinearRsss(short *a, short b, short c);
extern unsigned char _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(void *self, void *clsn, short *a);
extern void func_ov062_02117724(void *c, int a, int b, int d, int e);
extern void func_ov062_021181a0(void *c);
extern void func_ov062_0211811c(void *c);
extern void func_ov062_021180d4(void *c);
extern int func_ov062_02118058(char *c);
void func_ov062_02118cdc(char *c)
{
    if (*(unsigned char*)(c+0x3cc)) {
        *(unsigned char*)(c+0x3cc) = (_Z14ApproachLinearRsss((short*)(c+0x94), *(short*)(c+0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int*)(c+0x3b8) >= 0x61a8000) {
            *(short*)(c+0x3c2) = *(short*)(c+0x3c0);
        }
        *(unsigned char*)(c+0x3cc) = _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, c+0x144, (short*)(c+0x3c2));
        _Z14ApproachLinearRsss((short*)(c+0x94), *(short*)(c+0x3c2), 0x200);
    }
    if (*(unsigned char*)(c+0x398) == 1) {
        func_ov062_02117724(c, 2, 8, 0x13, 0x19);
    }
    switch (*(unsigned short*)(c+0x3c4)) {
    case 0: func_ov062_021181a0(c); break;
    case 1: func_ov062_0211811c(c); break;
    case 2: func_ov062_021180d4(c); break;
    }
    func_ov062_02118058(c);
}
