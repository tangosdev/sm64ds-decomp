//cpp
// NONMATCHING: register allocation (div=24). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;

struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct BTP_File;
struct BMA_File;
struct BTA_File;
struct Vector3;
struct Actor;
struct Vector3_16;

extern "C" {
    void func_ov074_02122634();
    void *_ZN5Model8LoadFileER13SharedFilePtr(void *shared);
    void LoadKeyModels(int idx);
    void *_ZN9Animation8LoadFileER13SharedFilePtr(void *shared);
    void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *shared);
    void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *mb, void *bmd, int a, int b);
    void _ZN11ShadowModel12InitCylinderEv(void *self);
    s32 func_01ffa344(s32 a);
    void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        void *self, void *actor, void *pos, s32 fx, s32 fy, u32 a, u32 b);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
    void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void *bmd, void *bma);
    void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta);
    void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        void *self, void *actor, s32 fa, s32 fb, void *v0, void *v1);
    void _ZN13RaycastGroundC1Ev(void *self);
    void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *actor);
    int _ZN13RaycastGround10DetectClsnEv(void *self);
    void _ZN13RaycastGroundD1Ev(void *self);
    void func_ov074_02121300(void *c);
    void func_ov074_0212195c(void *t);
}

extern void *data_ov002_0210da30;
extern void *data_ov084_02130cf8;
extern void *data_ov074_0212292c[];
extern void *data_ov074_02122948[];
extern void *data_ov074_02123040;
extern void *data_ov074_02123000;
extern void *data_ov074_02122e4c[];
extern void *data_ov074_02123030;
extern void *func_021123f4;
extern void *data_ov074_02122e5c[];
extern void *func_021124ac;
extern u16 data_ov074_02122e04[];
extern u16 data_ov074_02122dfc[];
extern s16 data_02082214[];

struct RG { char pad[0x4c]; };

extern "C" int _ZN8Goomboss13InitResourcesEv(char *self)
{
    s32 i;
    s32 j;
    s32 k;
    char *c5;
    char *c8;
    void *p;
    RG rg;
    s32 v[3];

    if (*(u32 *)(self + 8) == 0x1111) {
        func_ov074_02122634();
        return;
    }

    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
    LoadKeyModels(2);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov084_02130cf8);

    i = 0;
    do {
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov074_0212292c[i]);
        i++;
    } while (i < 7);

    i = 0;
    do {
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov074_02122948[i]);
        i++;
    } while (i < 0xc);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov074_02123040);
    p = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov074_02123000);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x210, p, 1, -1);

    j = 0;
    c5 = self + 0x274;
    do {
        _ZN11ShadowModel12InitCylinderEv(c5);
        j++;
        c5 += 0x28;
    } while (j < 3);

    *(u8 *)(self + 0x604) = 3;
    p = (void *)func_01ffa344((s32)data_ov074_02122e4c[*(u8 *)(self + 0x604)]);
    *(void **)(self + 0x80) = p;
    *(void **)(self + 0x84) = p;
    *(void **)(self + 0x88) = p;
    *(s32 *)(self + 0x9c) = -0xa000;
    *(s32 *)(self + 0xa0) = -0x3c000;

    *(s32 *)(self + 0x5d0) = *(s32 *)(self + 0x5c);
    *(s32 *)(self + 0x5d4) = *(s32 *)(self + 0x60);
    *(s32 *)(self + 0x5d8) = *(s32 *)(self + 0x64);

    c8 = self + 0x110;
    k = 0;
    do {
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            c8, self, self + 0x5c, *(s32 *)(self + 0x80) * 0xa, *(s32 *)(self + 0x84) * 0xa, 0x200004, 0x26fe0);
        k++;
        c8 += 0x40;
    } while (k < 4);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x210, *((void **)&data_ov074_02123030 + 1), 0, 0x1000, 0);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*((void **)&data_ov074_02123000 + 1), *((void **)&data_ov074_02123040 + 1));
    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(*((void **)&data_ov074_02123000 + 1), &func_021123f4);

    i = 0;
    do {
        _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*((void **)&data_ov074_02123000 + 1), data_ov074_02122e5c[i]);
        i++;
    } while (i < 7);

    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(self + 0x3e4, *((void **)&data_ov074_02123040 + 1), 0, 0x1000, 0);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3d0, &func_021123f4, 0x40000000, 0x1000, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(self + 0x3f8, &func_021124ac, 0, 0x1000, 0);

    *(s32 *)(self + 0x3f0) = 0;
    *(s32 *)(self + 0x3d8) = (s32)(data_ov074_02122e04[*(u8 *)(self + 0x604)] << 0xc);

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x40c, self, 0x14000, 0x14000, 0, 0);

    *(s32 *)(self + 0x5f0) = -1;
    *(s16 *)(self + 0x5f8) = (s16)(*(s32 *)(self + 0x5f0) * data_ov074_02122dfc[*(u8 *)(self + 0x604)]);
    *(s16 *)(self + 0x5f6) = 0;
    *(s16 *)(self + 0x5f4) = 0x1000;
    {
        s16 *pf = (s16 *)(self + 0x5f4);
        *pf = (s16)(*pf + *(s16 *)(self + 0x5f6));
    }

    *(s32 *)(self + 0x5c) = (s32)(((s64)data_02082214[(*(u16 *)(self + 0x5f4) >> 4) * 2] * 0x546000 + 0x800) >> 0xc);
    *(s32 *)(self + 0x64) = (s32)(((s64)data_02082214[(*(u16 *)(self + 0x5f4) >> 4) * 2 + 1] * 0x546000 + 0x800) >> 0xc);

    {
        s32 a2 = *(s32 *)(self + 0x60);
        s32 a1 = *(s32 *)(self + 0x64);
        s32 a0 = *(s32 *)(self + 0x5c);
        a2 = a2 + 0x64000;
        v[0] = a0;
        v[1] = a2;
        v[2] = a1;
    }

    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, v, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
        *(s32 *)(self + 0x60) = *(s32 *)((char *)&rg + 0x44);
    }

    func_ov074_02121300(self);
    func_ov074_0212195c(self);

    *(s32 *)(self + 0x5dc) = 0;
    *(u8 *)(self + 0x60a) = 1;
    *(s32 *)(self + 0x5e4) = 0x1000;

    _ZN13RaycastGroundD1Ev(&rg);

    return 1;
}
