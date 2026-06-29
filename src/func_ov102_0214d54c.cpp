//cpp
typedef int Fix12;
struct BMD_File;
struct SharedFilePtr { int h; };
struct Actor;
struct Vector3_16;

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
extern "C" void func_ov102_0214d1f8(void* c, void* p);

extern SharedFilePtr* data_ov102_0214d70c[];
extern void* data_ov102_0214ea68;

struct Obj {
    char p0[8];
    unsigned int f8;        /* 0x8 */
    char gc[0x5c - 0xc];
    int f5c, f60, f64;      /* 5c,60,64 */
    char g68[0x9c - 0x68];
    int f9c, fa0;           /* 9c, a0 */
    char ga4[0x100 - 0xa4];
    short f100;             /* 0x100 */
    char g102[0x110 - 0x102];
    char f110[0x144 - 0x110];   /* MovingCylinderClsn at 0x110 */
    char f144[0x300 - 0x144];   /* WithMeshClsn at 0x144 */
    char f300[0x350 - 0x300];
    char f350[0x3b0 - 0x350];
    int f3b0, f3b4, f3b8;   /* 3b0,3b4,3b8 */
    char g3bc[0x3c0 - 0x3bc];
    int f3c0;               /* 0x3c0 */
    unsigned char f3c4, f3c5;   /* 3c4,3c5 */
    char g3c6[0x3c8 - 0x3c6];
    int f3c8, f3cc, f3d0, f3d4;  /* 3c8,3cc,3d0,3d4 */
};

extern "C" int func_ov102_0214d54c(Obj* o)
{
    BMD_File* bmd;
    o->f3c4 = (unsigned char)(o->f8 & 1);
    o->f3c5 = (unsigned char)((o->f8 >> 4) & 1);
    bmd = Model::LoadFile(*data_ov102_0214d70c[o->f3c4]);
    if (((ModelBase*)&o->f300)->SetFile(bmd, 1, 1) == 0)
        return 0;
    if (((ShadowModel*)&o->f350)->InitCylinder() == 0)
        return 0;
    ((MovingCylinderClsn*)&o->f110)->Init((Actor*)o, 0x3c000, 0x46000, 0x100004, 0xa083c0);
    o->f3b0 = o->f5c;
    o->f3b4 = o->f60;
    o->f3b8 = o->f64;
    o->f9c = -0x2000;
    o->fa0 = -0x32000;
    o->f100 = 0x14;
    o->f3c0 = 0;
    ((WithMeshClsn*)&o->f144)->Init((Actor*)o, 0x28000, 0x28000, 0, 0);
    ((WithMeshClsn*)&o->f144)->StartDetectingWater();
    func_ov102_0214d1f8(o, &data_ov102_0214ea68);
    o->f3d4 = 0;
    o->f3d0 = o->f3d4;
    o->f3cc = o->f3d0;
    o->f3c8 = o->f3cc;
    return 1;
}
