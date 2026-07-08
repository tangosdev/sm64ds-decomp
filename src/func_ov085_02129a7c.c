//cpp
// NONMATCHING: different op / idiom (div=26). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

struct SharedFilePtr { u32 data[4]; };
struct BMD_File;

extern SharedFilePtr data_ov085_02130488;
extern SharedFilePtr data_ov085_02130490;
extern SharedFilePtr data_ov085_02130480;
extern SharedFilePtr MARIO_CAP_MODEL_PTR;
extern SharedFilePtr LUIGI_CAP_MODEL_PTR;
extern SharedFilePtr WARIO_CAP_MODEL_PTR;
extern u8 data_ov085_0212f27c[];

extern "C" {
extern BMD_File* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* p);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* p);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, Fix12 a, Fix12 b, u32 c, u32 d);
extern int func_02013a44(void);
extern void* _ZN5Actor13ClosestPlayerEv(void* thiz);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const Vector3* pos, const void* rot, int e, int f);
extern u8 NumStars(void);
extern int IsStarCollectedInCurLevel(int s);
extern void func_ov085_02129524(void* c, int i);
extern void _ZN13RaycastGroundC1Ev(void* thiz);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* thiz, const Vector3* pos, void* actor);
extern int _ZN13RaycastGround10DetectClsnEv(void* thiz);
extern void _ZN13RaycastGroundD1Ev(void* thiz);
}

struct RaycastGround { char buf[0x50]; };

extern "C" int func_ov085_02129a7c(char* r4)
{
    RaycastGround ray;
    Vector3 objPos;

    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_02130488);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_02130490);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(r4 + 0x108, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_02130480), 1, 0x16);
    _ZN11ShadowModel12InitCylinderEv(r4 + 0x16c);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(r4 + 0xd4, r4, 0x78000, 0x8c000, 0x4200004, 0);

    *(u8*)(r4 + 0x20d) = 0xff;
    *(u8*)(r4 + 0x20e) = 0xff;
    *(u16*)(r4 + 0x208) = *(s32*)(r4 + 8);
    *(u8*)(r4 + 0x20c) = (u8)*(s16*)(r4 + 0x8c);
    *(u8*)(r4 + 0x20b) = (u8)(*(s16*)(r4 + 0x8c) >> 8);
    if (*(u16*)(r4 + 0x208) == 0xffff)
        *(u16*)(r4 + 0x208) = 0;
    *(s32*)(r4 + 0x1f4) = 0;

    if (*(u8*)(r4 + 0x20b) == 1) {
        _ZN5Model8LoadFileER13SharedFilePtr(&MARIO_CAP_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&LUIGI_CAP_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&WARIO_CAP_MODEL_PTR);
        if (func_02013a44()) {
            void* p = _ZN5Actor13ClosestPlayerEv(r4);
            if (p) {
                void* spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0x10d, 0xd | (*(u8*)((char*)p + 0x6d9) << 8),
                    (Vector3*)(r4 + 0x5c), (void*)0, *(s8*)(r4 + 0xcc), -1);
                if (spawned)
                    *(s32*)(r4 + 0x1f4) = *(s32*)((char*)spawned + 4);
            }
        }
    }

    if (*(u8*)(r4 + 0x20c) != 0xff) {
        u16 v = *(u16*)(r4 + 0x208);
        switch (v) {
        case 0xaf5:
            *(u8*)(r4 + 0x20f) = 0;
            break;
        case 0xaf7:
            *(u8*)(r4 + 0x20f) = 1;
            break;
        case 0xaf9:
            *(u8*)(r4 + 0x20f) = 2;
            break;
        }

        if (!IsStarCollectedInCurLevel(*(u8*)(r4 + 0x20c))) {
            if (NumStars() < data_ov085_0212f27c[*(u8*)(r4 + 0x20f)]) {
                u16* cnt = (u16*)(r4 + 0x208);
                *cnt = *cnt + 1;
            }
        }
    }

    func_ov085_02129524(r4, 0);

    objPos.x = *(s32*)(r4 + 0x5c);
    objPos.y = *(s32*)(r4 + 0x60);
    objPos.z = *(s32*)(r4 + 0x64);
    objPos.y = objPos.y + 0x14000;

    _ZN13RaycastGroundC1Ev(&ray);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&ray, &objPos, (void*)0);
    if (_ZN13RaycastGround10DetectClsnEv(&ray))
        *(s32*)(r4 + 0x60) = *(s32*)((char*)&ray + 0x44);
    _ZN13RaycastGroundD1Ev(&ray);

    return 1;
}
