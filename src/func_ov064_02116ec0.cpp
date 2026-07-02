//cpp
typedef int Fix12;
struct BMD_File;
struct BCA_File;
struct Actor;
struct Vector3_16;

struct SharedFilePtr { short a, b; void* file; };

struct Animation { static void* LoadFile(SharedFilePtr& f); };
struct Model { static BMD_File* LoadFile(SharedFilePtr& f); };
struct ModelBase { int SetFile(BMD_File* f, int a, int b); };
struct ModelAnim { void SetAnim(BCA_File* f, int a, Fix12 t, unsigned int u); };
struct ShadowModel { int InitCylinder(); };
struct MovingCylinderClsn { void Init(Actor* a, Fix12 r, Fix12 h, unsigned int d, unsigned int e); };
struct WithMeshClsn { void Init(Actor* a, Fix12 b, Fix12 c, Vector3_16* d, Fix12 e); };

struct Desc {
    SharedFilePtr* f0;
    SharedFilePtr* f4;
    SharedFilePtr* f8;
    SharedFilePtr* fc;
    SharedFilePtr* f10;
    Fix12 f14;
    Fix12 f18;
    int f1c;
    int f20;
    Fix12 f24;
    int f28;
};

struct Obj {
    char p0[0xc];
    unsigned short fc;              /* 0x0c */
    char gc[0x5c - 0xe];
    int f5c, f60, f64;             /* 5c,60,64 */
    char g68[0x9c - 0x68];
    int f9c, fa0;                  /* 9c,a0 */
    char ga4[0x100 - 0xa4];
    short f100;                    /* 0x100 */
    char g102[0x110 - 0x102];
    char f110[0x174 - 0x110];      /* ModelBase/ModelAnim at 0x110 */
    char f174[0x330 - 0x174];      /* WithMeshClsn at 0x174 */
    Desc* f330;                    /* 0x330 */
    int f334, f338;                /* 334,338 */
    char f33c[0x358 - 0x33c];      /* MovingCylinderClsn at 0x33c */
    int f358;                      /* 0x358 */
    char g35c[0x370 - 0x35c];
    char f370[0x39c - 0x370];      /* ShadowModel at 0x370 */
    int f39c, f3a0, f3a4, f3a8, f3ac, f3b0;   /* 39c..3b0 */
    char g3b4[0x3e8 - 0x3b4];
    int f3e8, f3ec, f3f0;          /* 3e8,3ec,3f0 */
};

extern "C" int func_ov064_02116ec0(Obj* obj)
{
    BMD_File* bmd;

    Animation::LoadFile(*obj->f330->f4);
    Animation::LoadFile(*obj->f330->f8);
    Animation::LoadFile(*obj->f330->fc);
    Animation::LoadFile(*obj->f330->f10);
    bmd = Model::LoadFile(*obj->f330->f0);
    if (((ModelBase*)&obj->f110)->SetFile(bmd, 1, 1) == 0)
        return 0;
    if (((ShadowModel*)&obj->f370)->InitCylinder() == 0)
        return 0;
    ((ModelAnim*)&obj->f110)->SetAnim((BCA_File*)obj->f330->f10->file, 0, 0x1000, 0);
    ((MovingCylinderClsn*)&obj->f33c)->Init((Actor*)obj, obj->f330->f14, obj->f330->f18, 0x200000, 0x27c0);
    {
        int b = (obj->fc == 0xd9);
        if (b) {
            *(int *)(((int)obj + 0x358) & 0xFFFFFFFFFFFFFFFF) |= 0x40000;
        }
    }
    obj->f3a8 = obj->f5c;
    obj->f3ac = obj->f60;
    obj->f3b0 = obj->f64;
    obj->f39c = obj->f5c;
    obj->f3a0 = obj->f60;
    obj->f3a4 = obj->f64;
    obj->f9c = -0x4000;
    obj->fa0 = -0x32000;
    obj->f3e8 = obj->f330->f1c;
    obj->f3ec = obj->f330->f20;
    obj->f100 = 0;
    ((WithMeshClsn*)&obj->f174)->Init((Actor*)obj, obj->f330->f24, obj->f330->f24, 0, 0);
    obj->f3f0 = obj->f330->f28;
    obj->f338 = 0;
    obj->f334 = obj->f338;
    return 1;
}
