extern int _ZN9Animation8FinishedEv(void *anim);
extern void func_ov062_02117994(char *c, int idx);
extern int _Z14ApproachLinearRsss(short *a, short b, short c);
extern void *func_ov062_02117b9c(void *c);
extern int Vec3_Dist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern unsigned char _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(void *self, void *clsn, short *a);
extern int _ZN8dActor_c14GetSubtractionEss(void *self, short a, short b);
extern void _Z14ApproachLinearRiii(int *a, int b, int c);
extern int func_ov062_02118058(char *c);

void func_ov062_021183e0(char *c)
{
    int dist = 0x7fffffff;
    char *other;

    if (_ZN9Animation8FinishedEv(c + 0x350))
        func_ov062_02117994(c, 1);

    if (*(unsigned char*)(c + 0x3cc)) {
        *(unsigned char*)(c + 0x3cc) = (_Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int*)(c + 0x3b8) >= 0x61a8000)
            *(short*)(c + 0x3c2) = *(short*)(c + 0x3c0);
        other = func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(c + 0x5c, other + 0x5c);
            if (dist < 0xc8000)
                *(short*)(c + 0x3c2) = Vec3_HorzAngle(c + 0x5c, other + 0x5c);
        }
        *(unsigned char*)(c + 0x3cc) = _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, c + 0x144, (short*)(c + 0x3c2));
        _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x200);
    }

    if (dist < 0xc8000 && _ZN8dActor_c14GetSubtractionEss(c, *(short*)(c + 0x3c2), *(short*)(c + 0x94)) < 0xc00) {
        *(short*)(c + 0x94) = *(short*)(c + 0x3c2);
        *(int*)(c + 0x38c) = 2;
        *(int*)(c + 0x98) = 0x18000;
        *(int*)(c + 0xa8) = dist / 30;
        func_ov062_02117994(c, 6);
        *(short*)(c + 0x3c6) = 0x14;
        return;
    }

    if (*(int*)(c + 0x390) == 2)
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x2000, 0x4cc);
    else
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3000, 0x4cc);

    func_ov062_02118058(c);
}
