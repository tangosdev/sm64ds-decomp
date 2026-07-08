//cpp
// NONMATCHING: base materialization / addressing (div=53). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int Fix12;
struct Vector3 { int x, y, z; };
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50 - 0x14 - 0x30]; };
struct WithMeshClsn;
struct CylinderClsn;
struct ShadowModel;
struct Matrix4x3;

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor8PoofDustEv(void *self);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void *self);
extern void func_02012694(int a, void *b);
extern void _ZN13RaycastGroundC1Ev(RaycastGround *self);
extern void _ZN4BgCh19StartDetectingWaterEv(RaycastGround *self);
extern void _ZN4BgCh19StartDetectingToxicEv(RaycastGround *self);
extern void _ZN4BgCh21StopDetectingOrdinaryEv(RaycastGround *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround *self, const Vector3 *v, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround *self);
extern int func_02037e20(int *p);
extern void _ZN13RaycastGroundD1Ev(RaycastGround *self);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, CylinderClsn *cc);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm, unsigned int j);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, ShadowModel *sm, Matrix4x3 *mtx, Fix12 h, Fix12 g, unsigned int u);
extern struct Blob48 { int w[12]; } _ZN9Matrix3x38IDENTITYE;

int func_ov074_021223bc(char *c)
{
    RaycastGround rg;
    Vector3 pos;

    if (*(u8 *)(c + 0x609) == 0) {
        return 1;
    }

    if (DecIfAbove0_Short((u16 *)(c + 0x5fc)) == 0 || *(u8 *)(c + 0x603) >= 3) {
        int b = ((*(int *)(c + 0xb0) & 8) != 0) ? 1 : 0;
        if (b == 0) {
            _ZN5Actor8PoofDustEv(c);
        }
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x40c)) {
        int b = ((*(int *)(c + 0xb0) & 8) != 0) ? 1 : 0;
        if (b == 0) {
            if (*(u8 *)(c + 0x603) == 0) {
                func_02012694(0x13a, c + 0x74);
            } else {
                func_02012694(0x13b, c + 0x74);
            }
            {
                u8 *cnt = (u8 *)(c + 0x603);
                (*cnt)++;
            }

            *(int *)(c + 0xa8) = *(int *)(c + 0x98) + 0xa000;

            pos.x = *(int *)(c + 0x5c);
            pos.y = *(int *)(c + 0x60) + 0x190000;
            pos.z = *(int *)(c + 0x64);

            _ZN13RaycastGroundC1Ev(&rg);
            _ZN4BgCh19StartDetectingWaterEv(&rg);
            _ZN4BgCh19StartDetectingToxicEv(&rg);
            _ZN4BgCh21StopDetectingOrdinaryEv(&rg);
            _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, c);

            if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0
                && func_02037e20(rg.floor) != 0
                && rg.floor[(0x44 - 0x14) / 4] != (int)0x80000000
                && *(int *)(c + 0x60) < rg.floor[(0x44 - 0x14) / 4]) {
                _ZN5Actor8PoofDustEv(c);
                _ZN9ActorBase18MarkForDestructionEv(c);
                _ZN13RaycastGroundD1Ev(&rg);
                return 1;
            }
        }
    }

    if (*(u16 *)(c + 0x5fc) < 0x60) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn *)(c + 0x40c), 0);

        *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x8c) + 0x1800;
    }

    *(Blob48 *)(c + 0x2ec) = _ZN9Matrix3x38IDENTITYE;

    *(int *)(c + 0x310) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x314) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x318) = *(int *)(c + 0x64) >> 3;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (ShadowModel *)(c + 0x274), (Matrix4x3 *)(c + 0x2ec), 0x8c000, 0x3e8000, 0xf);

    _ZN13RaycastGroundD1Ev(&rg);

    return 1;
}
}
