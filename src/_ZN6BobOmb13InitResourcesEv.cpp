//cpp
// @symbol _ZN6BobOmb13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BobOmb.h"
typedef int Fix12;
struct BMD_File;
struct SharedFilePtr { int h; };
struct Actor;
struct Vector3_16;

struct Animation {
    static void LoadFile(SharedFilePtr& f);
};
struct Model {
    static BMD_File* LoadFile(SharedFilePtr& f);
};
struct ModelBase {
    int SetFile(BMD_File* f, int a, int b);
};
struct ShadowModel {
    int InitCylinder();
};
struct MovingCylinderClsn {
    void Init(Actor* a, Fix12 r, Fix12 h, unsigned int d, unsigned int e);
};
struct WithMeshClsn {
    void Init(Actor* a, Fix12 b, Fix12 c, Vector3_16* d, Fix12 e);
    void StartDetectingWater();
};
extern "C" void func_ov102_0214c0b8(void* c);

extern SharedFilePtr data_ov102_0214e9c0;
extern SharedFilePtr data_ov102_0214e9c8;
extern SharedFilePtr data_ov002_0210d9e0;

struct S48 { int a[12]; };
extern S48 data_02082128;

struct Obj {
    char p0[8];
    unsigned int f8;                    /* 0x8 */
    char g0c[0x5c - 0xc];
    int f5c, f60, f64;                  /* 5c,60,64 */
    char g68[0x80 - 0x68];
    int f80, f84, f88;                  /* 80,84,88 */
    char g8c[2];
    short f8e;                          /* 8e */
    char g90[0xa0 - 0x90];
    int fa0;                            /* a0 */
    char ga4[0xb0 - 0xa4];
    unsigned int fb0;                   /* b0 */
    char gb4[0xc8 - 0xb4];
    int fc8;                            /* c8 */
    char gcc[0x108 - 0xcc];
    unsigned char f108;                 /* 108 */
    char g109[0x110 - 0x109];
    char f110[0x18];                    /* MovingCylinderClsn at 0x110 */
    unsigned int f128;                  /* 128 */
    char g12c[0x144 - 0x12c];
    char f144[0x300 - 0x144];           /* WithMeshClsn at 0x144 */
    char f300[0x364 - 0x300];           /* ModelBase at 0x300 */
    char f364[0x390 - 0x364];           /* ShadowModel at 0x364 */
    unsigned int f390;                  /* 390 */
    S48 f394;                           /* 394..3c3 */
    int f3c4, f3c8, f3cc;               /* 3c4,3c8,3cc */
    char g3d0[0x3e0 - 0x3d0];
    int f3e0;                           /* 3e0 */
    char g3e4[4];
    unsigned short f3e8, f3ea, f3ec, f3ee;  /* 3e8..3ef */
    unsigned short f3f0;                /* 3f0 */
    unsigned char f3f2, f3f3, f3f4, f3f5, f3f6;  /* 3f2..3f6 */
};

int BobOmb::InitResources()
{
    BMD_File* bmd;

    Animation::LoadFile(data_ov102_0214e9c0);
    Animation::LoadFile(data_ov102_0214e9c8);
    bmd = Model::LoadFile(data_ov002_0210d9e0);
    if (((ModelBase*)&((Obj*)this)->f300)->SetFile(bmd, 1, -1) == 0)
        return 0;
    if (((ShadowModel*)&((Obj*)this)->f364)->InitCylinder() == 0)
        return 0;

    ((Obj*)this)->f3f5 = (unsigned char)(((Obj*)this)->f8 & 7);
    ((Obj*)this)->f3ec = 0x2000;
    func_ov102_0214c0b8(((Obj*)this));
    ((MovingCylinderClsn*)&((Obj*)this)->f110)->Init((Actor*)((Obj*)this), 0x3c000, 0x50000, 0x200004, 0xa6d380);
    ((Obj*)this)->fa0 = -0x37000;

    if (((Obj*)this)->f3f5 == 2) {
        *(unsigned int*)(((int)((Obj*)this) + 0x128) & 0xFFFFFFFFFFFFFFFF) |= 2;
        *(unsigned int*)(((int)((Obj*)this) + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~1u;
        ((Obj*)this)->f108 = 0;
    } else if (((Obj*)this)->f3f5 == 4) {
        ((Obj*)this)->f108 = 0;
    } else {
        ((Obj*)this)->f108 = 1;
    }

    ((Obj*)this)->f3c4 = ((Obj*)this)->f5c;
    ((Obj*)this)->f3c8 = ((Obj*)this)->f60;
    ((Obj*)this)->f3cc = ((Obj*)this)->f64;
    ((Obj*)this)->f394 = data_02082128;
    ((Obj*)this)->f3e8 = 0;
    ((Obj*)this)->f3ea = 0;
    ((Obj*)this)->f80 = 0x1000;
    ((Obj*)this)->f84 = 0x1000;
    ((Obj*)this)->f88 = 0x1000;
    ((Obj*)this)->f390 = 0;
    ((Obj*)this)->f3f2 = 0;
    ((WithMeshClsn*)&((Obj*)this)->f144)->Init((Actor*)((Obj*)this), 0x32000, 0x32000, 0, 0);
    ((WithMeshClsn*)&((Obj*)this)->f144)->StartDetectingWater();
    ((Obj*)this)->f3f3 = 1;
    ((Obj*)this)->fc8 = 0;
    ((Obj*)this)->f3e0 = 2;
    ((Obj*)this)->f3f6 = 0;
    ((Obj*)this)->f3f0 = ((Obj*)this)->f8e;
    return 1;
}
