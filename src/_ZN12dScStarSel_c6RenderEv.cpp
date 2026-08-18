//cpp
// @symbol _ZN12dScStarSel_c6RenderEv
// NONMATCHING: near-miss from DB (div=51)
/* dScStarSel_c::Render() -- vtable slot 9. extern "C" carries the literal
 * mangled name unmangled -- see include/dScStarSel_c.h. */
struct VBase {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void DoRender(int);
};

extern "C" {

extern void func_ov003_020ae238(char *sl);
extern void func_ov003_020ae0b0(char *sl);
extern void func_ov003_020adfc8(char *sl);
extern int func_ov003_020adf50(char *c);
extern int SublevelToLevel(int i);
extern int IsStarCollectedInLevel(signed char levelID, int starID);
extern unsigned int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int id);
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int b, void *attr, int x, int y, int a, int cc, void *m);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int x, int y, int a, int cc, int fx, int t, int e, int f);
extern void _ZN3OAM9RenderSubEP7OamAttrii(void *attr, int x, int y);
extern void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int p1, int p2, int p3, int p4, int p5, int p6, int b, void *m);
extern void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(void *eye, void *at, void *up, int b, void *out);
extern void _Z13CopyToViewMatPK9Matrix4x3(void *m);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToScale(void *m, int x, int y, int z);

extern signed char data_02092110[];
extern short data_02082214[];
extern void *data_ov003_020b132c;
extern void *data_ov003_020b1338;
extern void *data_ov003_020b1344;
extern int data_020a0db0;
extern unsigned char data_0209caa0[];
extern void *func_020aba70[];
extern void *func_020ab938;
extern void *func_020ab940;
extern void *func_020abb18[];
extern void *func_020abd78;
extern void *func_020abd80;
extern void *func_020abb34;
extern void *func_020abb54;
extern void *func_020abb74;
extern void *func_020abb94;
extern void *func_020abcb4[];
extern void *func_020abbb4;
extern void *func_020abbf4;

struct M { int w[12]; };
extern struct M data_020a0e68;

int _ZN12dScStarSel_c6RenderEv(char *sl)
{
    func_ov003_020ae238(sl);
    func_ov003_020ae0b0(sl);

    if (SublevelToLevel(data_02092110[0]) < 0xF) {
        int lvl;
        unsigned char v;
        func_ov003_020adfc8(sl);
        lvl = SublevelToLevel(data_02092110[0]);
        v = (unsigned char)(lvl + 1);
        if (v <= 0xF) {
            int c;
            if (v < 10) {
                c = 0x7C;
            } else {
                c = 0x77;
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020aba70[v / 10], c, 0x7A, 8, -1, 0);
                c += 9;
            }
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020aba70[v % 10], c, 0x7A, 8, -1, 0);
        }
    }

    if (SublevelToLevel(data_02092110[0]) <= 0xE) {
        unsigned char c114;
        int r8;
        int r7;
        int prod;

        _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
            data_02082214[0x16], data_02082214[0x17], 0x1555, 0x1000, 0x1388000, 0x1000, 1, 0);
        {
            struct M mtx;
            _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(&data_ov003_020b132c, &data_ov003_020b1338, &data_ov003_020b1344, 1, &mtx);
            _Z13CopyToViewMatPK9Matrix4x3(&mtx);
        }

        c114 = *(unsigned char *)(sl + 0x114);
        r7 = 1;
        prod = ((int)c114 - 1) * 0x18000;
        r8 = -(prod / 2);
        if ((int)c114 >= 1) {
            int sp1C = 0x1400;
            int sp20 = 1;
            int sp18 = 0x28000;
            do {
                int idx;
                char *dst;
                Matrix4x3_FromTranslation(&data_020a0e68, r8, sp18, 0);
                if ((r7 - 1) == *(unsigned char *)(sl + 0x115)) {
                    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, (short)(data_020a0db0 * 0x300));
                    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, sp1C, sp1C, sp1C);
                }
                idx = (IsStarCollectedInLevel(data_02092110[0], r7) == 0) ? sp20 : 0;
                dst = sl + 0x64 + idx * 0x50;
                *(struct M *)(dst + 0x1c) = data_020a0e68;
                ((VBase *)dst)->DoRender(0);
                r7 += 1;
                r8 += 0x18000;
            } while (r7 <= *(unsigned char *)(sl + 0x114));
        }

        if ((int)*(unsigned char *)(sl + 0x114) > 0) {
            int sbi = 0;
            do {
                if ((*(unsigned char *)(sl + 0x131) >> sbi) & 1) {
                    if (sbi == *(unsigned char *)(sl + 0x115)) {
                        _ZN3OAM9RenderSubEP7OamAttrii(&func_020ab938, *(unsigned char *)(sl + sbi + 0x11A), 0x18);
                    } else {
                        _ZN3OAM9RenderSubEP7OamAttrii(&func_020ab940, *(unsigned char *)(sl + sbi + 0x11A), 0x18);
                    }
                }
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020abb18[sbi], *(unsigned char *)(sl + sbi + 0x11A), 8, -1, -1, 0);
                sbi += 1;
            } while (sbi < *(unsigned char *)(sl + 0x114));
        }

        if (*(unsigned char *)(sl + 0x135) != 0) {
            unsigned char m133 = *(unsigned char *)(sl + 0x133);
            if (m133 == 0) {
                unsigned char i115 = *(unsigned char *)(sl + 0x115);
                _ZN3OAM9RenderSubEP7OamAttrii(&func_020abd78, *(unsigned char *)(sl + i115 + 0x11A), 6);
            } else if (m133 == 1) {
                int aidx = func_ov003_020adf50(sl);
                char *p = sl + aidx;
                _ZN3OAM9RenderSubEP7OamAttrii(&func_020abd80, *(unsigned char *)(p + 0x124) - 0x24, *(unsigned char *)(p + 0x128) - 8);
            } else {
                _ZN3OAM9RenderSubEP7OamAttrii(&func_020abd80, 0x50, *(unsigned char *)(sl + 0x12B) + 8);
            }
        }

        {
            unsigned char v139 = *(unsigned char *)(sl + 0x139);
            if (v139 == 0) {
                /* nothing */
            } else if (v139 == 1) {
                unsigned char v137 = *(unsigned char *)(sl + 0x137);
                switch (v137) {
                case 0:
                    *(int *)(((int)sl + 0x50) & 0xFFFFFFFFFFFFFFFFULL) += 0x400;
                    if (*(int *)(sl + 0x50) >= 0x1800) {
                        *(int *)(sl + 0x50) = 0x1800;
                        *(unsigned char *)(sl + 0x137) = 1;
                    }
                    break;
                case 1:
                    *(int *)(((int)sl + 0x50) & 0xFFFFFFFFFFFFFFFFULL) -= 0x400;
                    if (*(int *)(sl + 0x50) <= 0x1000) {
                        *(int *)(sl + 0x50) = 0x1000;
                        *(unsigned char *)(sl + 0x137) = 2;
                    }
                    break;
                case 2:
                    *(int *)(((int)sl + 0x50) & 0xFFFFFFFFFFFFFFFFULL) -= 0x80;
                    if (*(unsigned char *)(sl + 0x119) == 0) {
                        *(unsigned char *)(sl + 0x137) = 3;
                    }
                    break;
                case 3:
                    *(int *)(((int)sl + 0x50) & 0xFFFFFFFFFFFFFFFFULL) += 0x100;
                    *(short *)(((int)sl + 0x110) & 0xFFFFFFFFFFFFFFFFULL) += 0x1000;
                    break;
                }
                *(int *)(sl + 0x54) = *(int *)(sl + 0x50);
            } else if (v139 == 2) {
                short ang = *(short *)(sl + 0x112) - 0x4000;
                int tidx = (unsigned short)ang >> 4;
                int cosv = data_02082214[tidx * 2 + 1];
                *(int *)(sl + 0x58) = (0x1000 - cosv) * 0xA + 0x1000;
                *(int *)(sl + 0x5c) = 0x1000;
                *(short *)(sl + 0x112) = (*(short *)(sl + 0x112) + 0x1000) & 0x7FFF;
                if (*(short *)(sl + 0x112) == 0x4000) {
                    *(unsigned char *)(((int)sl + 0x13A) & 0xFFFFFFFFFFFFFFFFULL) ^= 1;
                }
                if (*(int *)(sl + 0x60) != 0) {
                    *(int *)(((int)sl + 0x60) & 0xFFFFFFFFFFFFFFFFULL) += 0x1000;
                    if (*(int *)(sl + 0x60) >= 0x10000) {
                        *(int *)(sl + 0x60) = 0;
                    }
                }
            }
        }

        switch (data_0209caa0[0x41]) {
        case 0:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &func_020abb34, 0x80, *(unsigned char *)(sl + 0x12B), -1, -1,
                *(int *)(sl + 0x50), *(int *)(sl + 0x54), (unsigned short)*(short *)(sl + 0x110), -1);
            break;
        case 1:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &func_020abb54, 0x80, *(unsigned char *)(sl + 0x12B), -1, -1,
                *(int *)(sl + 0x50), *(int *)(sl + 0x54), (unsigned short)*(short *)(sl + 0x110), -1);
            break;
        case 2:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &func_020abb74, 0x80, *(unsigned char *)(sl + 0x12B), -1, -1,
                *(int *)(sl + 0x50), *(int *)(sl + 0x54), (unsigned short)*(short *)(sl + 0x110), -1);
            break;
        case 3: {
            int rot3 = 0;
            int rot5 = 0;
            int rot6 = 0;
            int rot4 = 0;
            int rot1 = 0;
            int rot2 = 0;
            int rot0 = 0;
            int r8b;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &func_020abb94, 0x80, *(unsigned char *)(sl + 0x12B), -1, -1,
                *(int *)(sl + 0x50), *(int *)(sl + 0x54), (unsigned short)*(short *)(sl + 0x110), -1);
            r8b = 0;
            do {
                int slFlag = rot0;
                if (*(unsigned char *)(sl + 0x132) == r8b && *(unsigned char *)(sl + 0x118) != 0) {
                    slFlag = 3;
                }
                if (_ZN8SaveData19IsCharacterUnlockedEj(r8b) != 0) {
                    if (*(unsigned char *)(sl + 0x139) == 2) {
                        if (r8b == func_ov003_020adf50(sl)) {
                            if (*(unsigned char *)(sl + 0x13A) == 0) {
                                char *sb1 = sl + r8b;
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, func_020abcb4[r8b + 3],
                                    *(unsigned char *)(sb1 + 0x124), *(unsigned char *)(sb1 + 0x128), -1, -1,
                                    *(int *)(sl + 0x58), *(int *)(sl + 0x5c), rot1, -1);
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, func_020abcb4[*(unsigned char *)(sb1 + 0x12C)],
                                    *(unsigned char *)(sb1 + 0x124), *(unsigned char *)(sb1 + 0x128), -1, -1,
                                    *(int *)(sl + 0x58), *(int *)(sl + 0x5c), rot2, -1);
                            } else {
                                int sel = (r8b != 0) ? 3 : rot3;
                                char *p1 = sl + r8b;
                                char *p2 = sl + (r8b ^ sel);
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, func_020abcb4[*(unsigned char *)(p2 + 0x12C)],
                                    *(unsigned char *)(p1 + 0x124), *(unsigned char *)(p1 + 0x128), -1, -1,
                                    *(int *)(sl + 0x58), *(int *)(sl + 0x5c), rot4, -1);
                            }
                        } else {
                            int u60 = *(int *)(sl + 0x60);
                            if (u60 != 0) {
                                char *sb2 = sl + r8b;
                                int u60b;
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, func_020abcb4[r8b + 3],
                                    *(unsigned char *)(sb2 + 0x124), *(unsigned char *)(sb2 + 0x128), -1, -1,
                                    u60, u60, rot5, -1);
                                u60b = *(int *)(sl + 0x60);
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, func_020abcb4[*(unsigned char *)(sb2 + 0x12C)],
                                    *(unsigned char *)(sb2 + 0x124), *(unsigned char *)(sb2 + 0x128), -1, -1,
                                    u60b, u60b, rot6, -1);
                            }
                        }
                    } else {
                        char *sb3 = sl + r8b;
                        _ZN3OAM9RenderSubEP7OamAttrii(func_020abcb4[r8b + 3], *(unsigned char *)(sb3 + 0x124), slFlag + *(unsigned char *)(sb3 + 0x128));
                        _ZN3OAM9RenderSubEP7OamAttrii(func_020abcb4[*(unsigned char *)(sb3 + 0x12C)], *(unsigned char *)(sb3 + 0x124), slFlag + *(unsigned char *)(sb3 + 0x128));
                    }
                }
                r8b += 1;
            } while (r8b < 3);
            break;
        }
        }

        if ((unsigned char)*(unsigned char *)(sl + 0x130) <= 1 || data_0209caa0[0x41] != 3) {
            if (SublevelToLevel(data_02092110[0]) <= 0xE) {
                _ZN3OAM9RenderSubEP7OamAttrii(&func_020abbb4, 0x80, (*(short *)(sl + 0x10A) >> 8) + 0xA0);
            } else {
                _ZN3OAM9RenderSubEP7OamAttrii(&func_020abbf4, 0x80, (*(short *)(sl + 0x10A) >> 8) + 0xA0);
            }
        }
    }

    return 1;
}

}
