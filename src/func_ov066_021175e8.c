typedef struct Vec3 { int x, y, z; } Vec3;

extern void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
extern void func_02012694(int a, void *p);
extern void func_ov066_021166c8(void *thiz);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *r, void *t, int step);
extern int Vec3_Dist(const void *a, const void *b);
extern void func_ov066_02116ac4(void *c, int v);
extern int func_ov066_02116a68(void *self);
extern int Vec3_HorzDist(const void *a, const void *b);
extern int Vec3_ApproachHorz(void *out, void *a, int maxStep);
extern void _Z14ApproachLinearRiii(int *r, int target, int step);
extern int func_ov066_02116b78(void *c);
extern void func_ov066_021164ec(void *c);
extern int func_ov066_0211603c(void *c);
extern void func_ov066_02116390(void *c);
extern void func_ov066_021165cc(void *c);
extern int _ZN9Animation8FinishedEv(void *thiz);
extern void func_ov066_021162e8(void *c);
extern int _ZN4dBgW9IsEnabledEv(void *thiz);
extern void _ZN4dBgW7DisableEv(void *thiz);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *thiz, void *f, void *m, int fx, int s, void *clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
extern void func_020398fc(void *p);
extern void _ZN4dBgW6EnableEP8dActor_c(void *thiz, void *a);
extern void func_ov066_02119454(void *c, void *p);

extern unsigned char data_ov066_0211ae0c;
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211abe0;
extern unsigned char data_ov066_0211ae04;
extern char data_ov066_0211ae14[];
extern char data_ov066_0211aeac[];
extern char data_ov066_0211b06c;
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern int func_02112c08;
extern int func_02112d48;
extern int func_ov066_0211a35c;

int func_ov066_021175e8(void *thiz)
{
    char *c = (char *)thiz;
    Vec3 v;

    switch (*(int *)(c + 0x4a0)) {
    case 0:
        if (data_ov066_0211ae0c == *(int *)(c + 0x49c)) {
            char *p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
            if (p != 0) {
                Vec3 *pp = (Vec3 *)(((int)p + 0x5c));
                *(int *)(c + 0x4bc) = pp->x;
                *(int *)(c + 0x4c0) = pp->y;
                *(int *)(c + 0x4c4) = pp->z;
                *(int *)(((int)c + 0x4c4)) -= 0xc8000;
                if (*(int *)(c + 0x49c) == 1)
                    *(int *)(((int)c + 0x4bc)) += 0x58000;
                else
                    *(int *)(((int)c + 0x4bc)) -= 0x58000;
                if (*(int *)(c + 0x4c4) < -0xc52000)
                    *(int *)(c + 0x4c4) = -0xc52000;
                else if (*(int *)(c + 0x4c4) > -0x73a000)
                    *(int *)(c + 0x4c4) = -0x73a000;
            }
            func_02012694(0x144, c + 0x74);
            *(int *)(c + 0x4c0) = *(int *)(c + 0x4a8) + 0x1c2000;
            *(int *)(c + 0x4a0) = 1;
        } else if (*(unsigned short *)(c + 0x4d0) == 0) {
            func_ov066_021166c8(c);
            *(int *)(c + 0x4a0) = 4;
        }
        break;
    case 1:
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c + 0x5c, c + 0x4bc, 0x28000);
        if (Vec3_Dist(c + 0x5c, c + 0x4bc) <= 0x28000) {
            *(unsigned short *)(c + 0x4d0) = 0xa;
            *(int *)(c + 0xb0) = 0x2000000;
            *(int *)(c + 0x5c) = *(int *)(c + 0x4bc);
            *(int *)(c + 0x60) = *(int *)(c + 0x4c0);
            *(int *)(c + 0x64) = *(int *)(c + 0x4c4);
            *(int *)(c + 0x4c0) = *(int *)(c + 0x4a8);
            *(int *)(c + 0x4a0) = 2;
        }
        break;
    case 2:
        if (*(unsigned short *)(c + 0x4d0) == 0)
            _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c + 0x5c, c + 0x4bc, 0x32000);
        if (Vec3_Dist(c + 0x5c, c + 0x4bc) <= 0x32000) {
            func_ov066_02116ac4(c, 0x7d0000);
            *(int *)(c + 0x5c) = *(int *)(c + 0x4bc);
            *(int *)(c + 0x60) = *(int *)(c + 0x4c0);
            *(int *)(c + 0x64) = *(int *)(c + 0x4c4);
            *(unsigned short *)(c + 0x4d0) = 0xa;
            *(int *)(c + 0x4a0) = 7;
            if (func_ov066_02116a68(c) == 0) {
                char *p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
                if (p != 0) {
                    Vec3 *pp = (Vec3 *)(((int)p + 0x5c));
                    v.x = pp->x;
                    v.y = pp->y;
                    v.z = pp->z;
                    *(unsigned short *)(c + 0x4d0) = 0x24;
                    if (Vec3_HorzDist(c + 0x5c, &v) < 0x400000) {
                        if (v.x < *(int *)(c + 0x5c))
                            *(short *)(c + 0x94) = -0x4000;
                        else
                            *(short *)(c + 0x94) = 0x4000;
                        *(int *)(c + 0x4c8) = 0;
                        *(int *)(c + 0x98) = 0;
                        *(int *)(c + 0x4a0) = 3;
                    }
                }
            }
        }
        break;
    case 3:
        if (*(int *)(c + 0x4c8) < 0x2710) {
            if (*(unsigned short *)(c + 0x4d0) != 0)
                *(int *)(((int)c + 0x4c8)) += 0x1a;
            else
                *(int *)(((int)c + 0x4c8)) += 0x130;
        }
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0x258000, *(int *)(c + 0x4c8));
        if (func_ov066_02116b78(c) == 1) {
            *(int *)(c + 0x98) = 0;
            *(short *)(c + 0x94) = 0;
            *(int *)(c + 0x4a0) = 7;
        }
        break;
    case 4:
        func_ov066_021164ec(c);
        if ((unsigned short)(*(int *)(c + 0x3b8) >> 0xc) == 0)
            func_02012694(0x140, c + 0x74);
        if (*(int *)(c + 0x498) == 1) {
            int r = func_ov066_0211603c(c);
            func_ov066_02116390(c);
            if (r != 0) {
                if (r == 1)
                    *(int *)(c + 0x4a0) = 5;
                break;
            }
        }
        if (data_ov066_0211ae08 != 0) {
            func_ov066_021165cc(c);
            *(int *)(c + 0x4a0) = 6;
        }
        break;
    case 5:
        if (_ZN9Animation8FinishedEv(c + 0x3b0) != 0) {
            func_ov066_021165cc(c);
            *(int *)(c + 0x4a0) = 6;
        }
        break;
    case 6:
        if (_ZN9Animation8FinishedEv(c + 0x3b0) != 0) {
            func_ov066_021162e8(c);
            if (_ZN4dBgW9IsEnabledEv(c + 0x674) != 0)
                _ZN4dBgW7DisableEv(c + 0x674);
            if (*(int *)(c + 0x49c) == 1)
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    c + 0x674, *(void **)(data_ov066_0211ae14 + 4), c + 0x83c, 0x199,
                    *(short *)(c + 0x8e), &func_02112c08);
            else
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    c + 0x674, *(void **)(data_ov066_0211aeac + 4), c + 0x83c, 0x199,
                    *(short *)(c + 0x8e), &func_02112d48);
            func_020393d4(c + 0x674, &_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
            func_020393c4(c + 0x674, &func_ov066_0211a35c);
            func_020398fc(c + 0x674);
            _ZN4dBgW6EnableEP8dActor_c(c + 0x674, c);
            data_ov066_0211ae08 += 1;
            *(int *)(c + 0x4a0) = 8;
        }
        break;
    case 7:
        if (*(unsigned short *)(c + 0x4d0) == 0) {
            Vec3_ApproachHorz(c + 0x5c, c + 0x4a4, 0x28000);
            if (Vec3_HorzDist(c + 0x5c, c + 0x4a4) <= 0x28000) {
                data_ov066_0211ae0c ^= *(int *)(c + 0x49c);
                *(int *)(c + 0x5c) = *(int *)(c + 0x4a4);
                *(int *)(c + 0x60) = *(int *)(c + 0x4a8);
                *(int *)(c + 0x64) = *(int *)(c + 0x4ac);
                data_ov066_0211ae08 += 1;
                if (data_ov066_0211abe0 != 3)
                    data_ov066_0211ae08 += 1;
                *(int *)(c + 0x4a0) = 8;
            }
        }
        break;
    case 8:
        if (data_ov066_0211ae04 != 7) {
            *(int *)(c + 0xb0) = 0;
            func_ov066_02119454(c, &data_ov066_0211b06c);
        }
        break;
    }
    return 1;
}
