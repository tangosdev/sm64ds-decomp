//cpp
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
extern SharedFilePtr BOB_OMB_MODEL_PTR;

struct S48 { int a[12]; };
extern S48 _ZN9Matrix3x38IDENTITYE;

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

extern "C" int func_ov102_0214c510(Obj* o)
{
    BMD_File* bmd;

    Animation::LoadFile(data_ov102_0214e9c0);
    Animation::LoadFile(data_ov102_0214e9c8);
    bmd = Model::LoadFile(BOB_OMB_MODEL_PTR);
    if (((ModelBase*)&o->f300)->SetFile(bmd, 1, -1) == 0)
        return 0;
    if (((ShadowModel*)&o->f364)->InitCylinder() == 0)
        return 0;

    o->f3f5 = (unsigned char)(o->f8 & 7);
    o->f3ec = 0x2000;
    func_ov102_0214c0b8(o);
    ((MovingCylinderClsn*)&o->f110)->Init((Actor*)o, 0x3c000, 0x50000, 0x200004, 0xa6d380);
    o->fa0 = -0x37000;

    if (o->f3f5 == 2) {
        *(unsigned int*)(((int)o + 0x128) & 0xFFFFFFFFFFFFFFFF) |= 2;
        *(unsigned int*)(((int)o + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~1u;
        o->f108 = 0;
    } else if (o->f3f5 == 4) {
        o->f108 = 0;
    } else {
        o->f108 = 1;
    }

    o->f3c4 = o->f5c;
    o->f3c8 = o->f60;
    o->f3cc = o->f64;
    o->f394 = _ZN9Matrix3x38IDENTITYE;
    o->f3e8 = 0;
    o->f3ea = 0;
    o->f80 = 0x1000;
    o->f84 = 0x1000;
    o->f88 = 0x1000;
    o->f390 = 0;
    o->f3f2 = 0;
    ((WithMeshClsn*)&o->f144)->Init((Actor*)o, 0x32000, 0x32000, 0, 0);
    ((WithMeshClsn*)&o->f144)->StartDetectingWater();
    o->f3f3 = 1;
    o->fc8 = 0;
    o->f3e0 = 2;
    o->f3f6 = 0;
    o->f3f0 = o->f8e;
    return 1;
}
