extern int LoadKeyModels();
extern int _ZN9Animation8LoadFileER13SharedFilePtr();
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii();
extern int _ZN5Model8LoadFileER13SharedFilePtr();
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii();
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj();
extern int _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj();
extern int _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_();
extern int LoadBlueCoinModel();
extern int _ZN8CapEnemy6AddCapEj();
extern int _ZN8CapEnemy21DestroyIfCapNotNeededEv();
extern int NumStars();
extern int _ZN9ActorBase18MarkForDestructionEv();
extern int IsStarCollectedInCurLevel();
extern int _ZN11ShadowModel12InitCylinderEv();
extern int func_02035800();
extern int _ZN5Actor13ClosestPlayerEv();
extern int Matrix4x3_FromTranslation();
extern int MulMat4x3Mat4x3();
extern int SubVec3();

struct G { int w[2]; };
extern struct G data_ov063_0211edd4;
extern struct G data_ov063_0211edcc;
extern struct G data_ov063_0211ede4;
extern struct G data_ov063_0211edf4;
extern struct G data_ov063_0211eddc;
extern struct G data_ov063_0211edc4;
extern struct G data_ov063_0211edec;
extern struct G COIN_BLUE_POLY32_MODEL_PTR;
extern struct G COIN_RED_POLY32_MODEL_PTR;
extern struct G COIN_YELLOW_POLY32_MODEL_PTR;
extern signed char LEVEL_ID;
extern unsigned char LAST_ENTRANCE_ID;
extern int data_ov063_0211e22c[];
extern int data_ov063_0211e1ec[];
struct H { int w[16]; };
extern struct H MATRIX_SCRATCH_PAPER;

#define U8(o)  (*(unsigned char *)(c + (o)))
#define S8(o)  (*(signed char *)(c + (o)))
#define U16(o) (*(unsigned short *)(c + (o)))
#define S16(o) (*(short *)(c + (o)))
#define U32(o) (*(unsigned int *)(c + (o)))
#define S32(o) (*(int *)(c + (o)))
#define PTR(o) (*(char **)(c + (o)))
#define FLAGS16 (*(unsigned short *)((long long)(c + 0x5d4) & 0xFFFFFFFFFFFFFFFFLL))
#define FLAGS16T (*(unsigned short *)((long long)((char *)((long long)c & 0xFFFFFFFFFFFFFFFFLL) + 0x5d4) & 0xFFFFFFFFFFFFFFFFLL))

int func_ov063_0211b9bc(char *c)
{
    int cond;
    int tmp598;
    void *spawned;
    void *pp;

    U16(0x5d4) = 0;
    S32(0x49c) = 0;
    cond = 0;
    if (U16(0xc) == 0xd2) cond = 1;
    if (cond) {
        U8(0x5cf) = (U32(8) & 0xf) + 0xc;
        if (U8(0x5cf) == 0xf) {
            LoadKeyModels(3);
            _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov063_0211edd4);
            _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov063_0211edcc);
        } else if (U8(0x5cf) == 0xc) {
            PTR(0x48c) = (char *)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xd3, U32(8), c + 0x5c, 0, S8(0x5d0), -1);
            if (PTR(0x48c) != 0) {
                S32(0x49c) = *(int *)(PTR(0x48c) + 4);
            }
            PTR(0x48c) = 0;
        }
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov063_0211ede4);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x380, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov063_0211edf4), 1, 1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380, data_ov063_0211ede4.w[1], 0, 0x1000, 0);
        U8(0x5ca) = 3;
        S32(0x590) = 0xc8;
        S32(0x594) = 0x104;
        S32(0x598) = -0x14000;
        tmp598 = *(volatile int *)(c + 0x598);
        S32(0x534) = 0;
        S32(0x538) = 0;
        S32(0x53c) = tmp598;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x184, c, c + 0x534, S32(0x590) << 0xc, S32(0x594) << 0xc, 0x200000, 0x207e0);
        if (U8(0x5cf) != 0xf) {
            _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1c4, c, 0xdc000, 0xdc000, 0, 0);
        } else {
            _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1c4, c, 0xc8000, 0xb4000, 0, 0);
        }
    } else {
        U8(0x5cf) = U32(8) & 0xf;
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov063_0211eddc);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x380, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov063_0211edc4), 1, 0x16);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380, data_ov063_0211eddc.w[1], 0, 0x1000, 0);
        U8(0x5ca) = 1;
        if (U8(0x5cf) == 5) {
            S32(0x598) = -0x24000;
            tmp598 = *(volatile int *)(c + 0x598);
            S32(0x534) = 0;
            S32(0x538) = 0;
            S32(0x53c) = tmp598;
            S32(0x590) = 0x43;
            S32(0x594) = 0x5a;
        } else {
            S32(0x598) = -0x14000;
            tmp598 = *(volatile int *)(c + 0x598);
            S32(0x534) = 0;
            S32(0x538) = 0;
            S32(0x53c) = tmp598;
            S32(0x590) = 0x4a;
            S32(0x594) = 0x64;
        }
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x184, c, c + 0x534, S32(0x590) << 0xc, S32(0x594) << 0xc, 0x200000, 0x207e0);
        if (LEVEL_ID == 0xc && S32(0x5c) == 0xbb8000 && S8(0xcc) == 2) {
            FLAGS16 = (FLAGS16 & ~1) | 1;
        }
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1c4, c, 0x32000, 0x32000, 0, 0);
    }

    U16(0x4a0) = 0x187;
    if (U8(0x5cf) == 5) {
        FLAGS16 |= 2;
        U16(0x4a0) = 0xd4;
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov063_0211edec);
    } else if (U8(0x5cf) == 0 || U8(0x5cf) == 1 || U8(0x5cf) == 2 || U8(0x5cf) == 6 || (unsigned)(unsigned char)(U8(0x5cf) + 0xf8) <= 3) {
        *(int *)((long long)(c + 0x1a0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x8000;
        FLAGS16 |= 2;
        if (U8(0x5cf) == 6) {
            U16(0x4a0) = 0x120;
        } else if ((unsigned)(unsigned char)(U8(0x5cf) + 0xf6) <= 1) {
            U16(0x4a0) = 0x121;
        } else {
            U16(0x4a0) = 0x122;
            LoadBlueCoinModel(c);
        }
    }

    U8(0x113) = 6;
    S32(0x564) = 0;
    S32(0x568) = 0x60000;
    S32(0x56c) = 0;
    cond = 0;
    if (U16(0xc) == 0xd1) cond = 1;
    if (cond && U8(0x5cf) != 8) {
        unsigned char capIdx;
        U8(0x112) = (U32(8) >> 0xc) & 0xf;
        capIdx = (U32(8) >> 8) & 0xf;
        _ZN8CapEnemy6AddCapEj(c, capIdx);
        if ((U8(0x113) & 7) < 6) {
            U32(8) = U32(8) & 0xfff;
        }
        if (_ZN8CapEnemy21DestroyIfCapNotNeededEv(c) == 0) {
            return 0;
        }
    }

    if ((unsigned)(unsigned char)(U8(0x5cf) + 0xf6) <= 1) {
        if ((unsigned)NumStars() < 3) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return 0;
        }
        if (U8(0x5cf) == 0xb && (unsigned)NumStars() >= 0xf) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return 0;
        }
        if (IsStarCollectedInCurLevel(1) != 0) {
            U16(0x4a0) = 0x120;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x434) == 0) return 0;
    if (_ZN11ShadowModel12InitCylinderEv(c + 0x45c) == 0) return 0;

    S32(0x51c) = S32(0x5c);
    S32(0x520) = S32(0x60);
    S32(0x524) = S32(0x64);
    S16(0x570) = S16(0x8c);
    S16(0x572) = S16(0x8e);
    S16(0x574) = S16(0x90);
    U16(0x100) = 0;
    S32(0x80) = 0x1000;
    S32(0x84) = 0x1000;
    S32(0x88) = 0x1000;
    U8(0x5c8) = 0xff;
    func_02035800(c + 0x1c4);
    U8(0x5cc) = 0;
    S16(0x5b2) = S16(0x94);
    U16(0x5b8) = 0;
    FLAGS16 |= 4;
    S32(0x490) = 0;
    S32(0x494) = 0;
    S32(0x498) = 0;
    S32(0x488) = 0;
    S32(0x5a0) = 0;
    U16(0x5b8) = 0;
    S32(0x5a4) = data_ov063_0211e22c[U8(0x5cf)];
    S32(0x5ac) = data_ov063_0211e1ec[U8(0x5cf)];
    S32(0x5a8) = 0;
    FLAGS16 |= 8;
    S32(0x180) = 0;
    S32(0xa0) = -0x3c000;
    S8(0x5d0) = S8(0xcc);
    U16(0x5c6) = 0;
    U16(0x5c2) = 0;
    U16(0x5c4) = 0;
    S32(0x59c) = ((U32(8) >> 8) & 0xff) * 0x64000;
    U8(0x5d3) = 0;
    S32(0x510) = 0xc00;
    S32(0x514) = 0xc00;
    S32(0x518) = 0xc00;

    if (U8(0x5cf) == 5) {
        if ((unsigned)NumStars() < 0xf) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return 1;
        }
        if (LAST_ENTRANCE_ID == 0) {
            spawned = (void *)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(U16(0x4a0), 0, c + 0x5c, 0, S8(0x5d0), -1);
            if (spawned != 0) {
                *(unsigned char *)((char *)spawned + 0x37e) = 1;
                _ZN9ActorBase18MarkForDestructionEv(c);
                return 1;
            }
        }
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x3e4, data_ov063_0211edec.w[1], 1, -1);
    } else if (U8(0x5cf) == 0 || U8(0x5cf) == 1 || U8(0x5cf) == 2 || U8(0x5cf) == 6 || (unsigned)(unsigned char)(U8(0x5cf) + 0xf8) <= 3) {
        unsigned short t = U16(0x4a0);
        if (t == 0x122) {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x3e4, COIN_BLUE_POLY32_MODEL_PTR.w[1], 1, -1);
        } else if (t == 0x121) {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x3e4, COIN_RED_POLY32_MODEL_PTR.w[1], 1, -1);
        } else {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x3e4, COIN_YELLOW_POLY32_MODEL_PTR.w[1], 1, -1);
        }
        if (U8(0x5cf) == 0xb) {
            S8(0xcc) = -1;
            pp = (void *)_ZN5Actor13ClosestPlayerEv(c);
            if (pp != 0 && *(int *)((char *)pp + 0x64) > (int)0xffaec000) {
                FLAGS16T |= 0x10;
            }
        }
    } else {
        if ((unsigned)U8(0x5cf) < 0xc) {
            *(int *)((long long)(c + 0x19c) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        }
        if (U8(0x5cf) == 4) {
            S8(0xcc) = -1;
            pp = (void *)_ZN5Actor13ClosestPlayerEv(c);
            if (pp != 0 && *(int *)((char *)pp + 0x64) > (int)0xffaec000) {
                FLAGS16T |= 0x10;
            }
        } else if (U8(0x5cf) == 7) {
            FLAGS16T |= 2;
            U16(0x4a0) = 0x120;
        } else if (U8(0x5cf) == 0xe) {
            S8(0xcc) = -1;
        }
    }

    S32(0x540) = 0;
    S32(0x544) = 0;
    S32(0x548) = 0;
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, S32(0x5c), S32(0x60), S32(0x64));
    MulMat4x3Mat4x3(S32(0x394), &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    S32(0x540) = MATRIX_SCRATCH_PAPER.w[9];
    S32(0x544) = MATRIX_SCRATCH_PAPER.w[10];
    S32(0x548) = MATRIX_SCRATCH_PAPER.w[11];
    SubVec3(c + 0x540, c + 0x5c, c + 0x540);
    U8(0x5d1) = 0;
    S16(0x5bc) = S16(0x94);
    S32(0x5dc) = 0;
    S32(0x5d8) = S32(0x5dc);
    return 1;
}
