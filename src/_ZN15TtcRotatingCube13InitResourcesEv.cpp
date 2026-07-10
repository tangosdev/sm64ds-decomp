//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;
struct Vector3;
struct Actor;

extern "C" {
    void *_ZN5Model8LoadFileER13SharedFilePtr(void *shared);
    void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *mb, void *bmd, int a, int b);
    void _ZN11ShadowModel10InitCuboidEv(void *self);
    void func_ov065_0211990c(void *self);
    void func_ov065_021198a0(void *self);
    void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *shared);
    void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *mc, void *kcl, void *mtx, int fix, s16 s, void *clps);
    void func_020393d4(void *p, void *v);
    void Matrix4x3_FromRotationY(void *m, int angle);
    void MulVec3Mat4x3(void *dst, void *mtx, void *src);
    void AddVec3(void *a, void *b, void *c);
    void _ZN13RaycastGroundC1Ev(void *self);
    void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *actor);
    int _ZN13RaycastGround10DetectClsnEv(void *self);
    void _ZN13RaycastGroundD1Ev(void *self);
}

extern void *data_ov065_0211cfd0[];
extern void *data_ov065_0211c0a8[];
extern s16 data_ov065_0211cfa8[];
extern void *data_ov065_0211cfd4[];
extern void *data_ov065_0211cfd8[];
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
extern u8 data_0209f2c0;
extern u8 data_ov065_0211cfa4[];
extern s32 data_020a0e68[];

struct RaycastGround {
    char pad[0x44];
    s32 hitY;
    char pad2[0xc];
};

extern "C" int _ZN15TtcRotatingCube13InitResourcesEv(void *self)
{
    u8 *c = (u8 *)self;
    u16 id = *(u16 *)(c + 0xc);

    if (id != 0x6c) {
        if (id == 0x6d) {
            *(u8 *)(c + 0x377) = 1;
        }
    } else {
        *(u8 *)(c + 0x377) = 0;
    }

    u8 idx = *(u8 *)(c + 0x377);
    {
        void *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov065_0211cfd0 + idx * 0xc));
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, bmd, 1, -1);
    }

    {
        void *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211c0a8[*(u8 *)(c + 0x377)]);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x320, bmd, 1, -1);
    }

    _ZN11ShadowModel10InitCuboidEv(c + 0x380);

    *(s16 *)(c + 0x300 + 0x78) = data_ov065_0211cfa8[*(u8 *)(c + 0x377)];

    func_ov065_0211990c(self);
    func_ov065_021198a0(self);

    idx = *(u8 *)(c + 0x377);
    if (idx == 0) {
        s32 oi = idx * 0xc;
        void *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov065_0211cfd4 + oi));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            c + 0x124, kcl, c + 0x2ec, 0x1000, *(s16 *)(c + 0x8e),
            *(void **)((char *)data_ov065_0211cfd8 + oi));
    } else {
        s32 oi = idx * 0xc;
        void *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov065_0211cfd4 + oi));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            c + 0x124, kcl, c + 0x2ec, 0x199, *(s16 *)(c + 0x8e),
            *(void **)((char *)data_ov065_0211cfd8 + oi));
    }

    func_020393d4(c + 0x124, (void *)_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    {
        s32 va[3];
        s32 vb[3];
        *(u16 *)(c + 0x300 + 0x74) = data_ov065_0211cfa4[data_0209f2c0];
        va[0] = 0;
        va[2] = 0;
        vb[0] = 0;
        vb[1] = 0;
        vb[2] = 0;
        va[1] = 0;
        va[0] = 0x64000;
        va[2] = 0x64000;

        Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
        MulVec3Mat4x3(va, data_020a0e68, vb);
        AddVec3(vb, c + 0x5c, vb);
        vb[1] -= 0xd2000;

        RaycastGround rg;
        _ZN13RaycastGroundC1Ev(&rg);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, vb, 0);
        *(s32 *)(c + 0x37c) = vb[1];
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            *(s32 *)(c + 0x37c) = rg.hitY;
        }

        vb[0] = *(s32 *)(c + 0x5c);
        vb[1] = *(s32 *)(c + 0x60);
        vb[2] = *(s32 *)(c + 0x64);
        vb[1] -= 0xd2000;

        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, vb, 0);
        s32 r5 = vb[1];
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            r5 = rg.hitY;
        }
        if (r5 != *(s32 *)(c + 0x37c)) {
            *(u8 *)(c + 0x37a) = 1;
        }
        _ZN13RaycastGroundD1Ev(&rg);
    }

    return 1;
}
