extern void func_ov062_02117724(void *c, int a, int b, int d, int e);
extern int _Z14ApproachLinearRsss(short *a, short b, short c);
extern void *func_ov062_02117b9c(void *c);
extern int Vec3_Dist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern unsigned char _ZN5Enemy24AngleAwayFromWallOrCliffER12WithMeshClsnRs(void *self, void *clsn, short *a);
extern int RandomIntInternal(int *seed);
extern int _ZN5Actor14GetSubtractionEss(void *self, short a, short b);
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern void func_ov062_02117994(char *c, int idx);
extern int data_0209e650;

void func_ov062_02118718(char *c)
{
    int dist = 0x7fffffff;
    char *other;

    if (*(unsigned char*)(c + 0x398) == 3)
        func_ov062_02117724(c, 2, 5, 8, 0xb);

    if (*(unsigned char*)(c + 0x3cc)) {
        *(unsigned char*)(c + 0x3cc) =
            (_Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int*)(c + 0x3b8) >= 0x61a8000)
            *(short*)(c + 0x3c2) = *(short*)(c + 0x3c0);

        other = func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(c + 0x5c, other + 0x5c);
            *(short*)(c + 0x3c2) = Vec3_HorzAngle(c + 0x5c, other + 0x5c);
        } else {
            *(unsigned char*)(c + 0x3cc) =
                _ZN5Enemy24AngleAwayFromWallOrCliffER12WithMeshClsnRs(c, c + 0x144, (short*)(c + 0x3c2));
            if (!*(unsigned char*)(c + 0x3cc)) {
                if (*(unsigned short*)(c + 0x3c8) != 0) {
                    *(unsigned short*)(((int)c + 0x3c8) & 0xFFFFFFFFFFFFFFFF) -= 1;
                } else {
                    if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x8000)
                        *(short*)(((int)c + 0x3c2) & 0xFFFFFFFFFFFFFFFF) -= ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    else
                        *(short*)(((int)c + 0x3c2) & 0xFFFFFFFFFFFFFFFF) += ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    *(unsigned short*)(c + 0x3c8) = 0x14;
                }
            }
        }

        if (*(int*)(c + 0x3b8) > 0x320000 ||
            (other != 0 &&
             _ZN5Actor14GetSubtractionEss(c, *(short*)(c + 0x3c2),
                 (short)(*(short*)(c + 0x94) + 0x8000)) < 0x2000)) {
            _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x600);
        } else {
            if (*(int*)(c + 0x394) != 0)
                _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c0), 0x600);
            else
                _Z14ApproachLinearRsss((short*)(c + 0x94), (short)(*(short*)(c + 0x3c0) + 0x8000), 0x600);
        }
    }

    if (_Z14ApproachLinearRiii((int*)(c + 0x98), 0x14000, 0x1000) == 0)
        return;
    if (dist >= 0xc8000)
        return;
    if (_ZN5Actor14GetSubtractionEss(c, *(short*)(c + 0x3c2), *(short*)(c + 0x94)) >= 0xc00)
        return;

    *(short*)(c + 0x94) = *(short*)(c + 0x3c2);
    *(int*)(c + 0x38c) = 2;
    *(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFF) += *(int*)(c + 0x98) / 5;
    *(int*)(c + 0xa8) = dist / 30;
    func_ov062_02117994(c, 6);
    *(short*)(c + 0x3c6) = 0x14;
}
