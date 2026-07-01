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
};

extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov098_0213c91c;

struct Obj {
    char p0[8];
    unsigned int f8;        /* 0x8 */
    char gc[0x5c - 0xc];
    int f5c, f60, f64;      /* 5c,60,64 */
    char g68[0x80 - 0x68];
    int f80, f84, f88;      /* 80,84,88 */
    char g8c[0x9c - 0x8c];
    int f9c, fa0;           /* 9c, a0 */
    char ga4[0x100 - 0xa4];
    short f100;             /* 0x100 */
    char g102[0x110 - 0x102];
    char f110[0x144 - 0x110];   /* MovingCylinderClsn at 0x110 */
    char f144[0x300 - 0x144];   /* WithMeshClsn at 0x144 */
    char f300[0x350 - 0x300];
    char f350[0x3a8 - 0x350];   /* ShadowModel at 0x350 */
    int f3a8, f3ac, f3b0;   /* 3a8,3ac,3b0 */
    char g3b4[0x3b6 - 0x3b4];
    unsigned char f3b6;     /* 0x3b6 */
    char g3b7[0x3c4 - 0x3b7];
    int f3c4;                /* 0x3c4 */
    int f3c8;                /* 0x3c8 */
};

extern "C" int func_ov098_0213bd14(Obj* o)
{
    BMD_File* bmd;
    int kind;

    kind = o->f8 & 0xf;
    o->f3c8 = kind;

    if (o->f3c8 == 2)
    {
        bmd = Model::LoadFile(data_ov002_0210da38);
        if (((ModelBase*)&o->f300)->SetFile(bmd, 1, 0x16) == 0)
            return 0;
    }
    else
    {
        Model::LoadFile(data_ov002_0210da38);
        bmd = Model::LoadFile(data_ov098_0213c91c);
        if (((ModelBase*)&o->f300)->SetFile(bmd, 1, 0x16) == 0)
            return 0;
    }

    if (((ShadowModel*)&o->f350)->InitCylinder() == 0)
        return 0;

    if (o->f3c8 == 0)
        o->f3c4 = 0;
    else
        o->f3c4 = 1;

    if (o->f3c8 != 0)
    {
        if (o->f3c8 == 2)
        {
            o->f80 = 0x800;
            o->f84 = 0x800;
            o->f88 = 0x800;
            ((MovingCylinderClsn*)&o->f110)->Init((Actor*)o, 0x14000, 0x28000, 0x200004, 0);
            ((WithMeshClsn*)&o->f144)->Init((Actor*)o, 0x1e000, 0x1e000, 0, 0);
        }
        else
        {
            o->f80 = 0x1000;
            o->f84 = 0x1000;
            o->f88 = 0x1000;
            ((MovingCylinderClsn*)&o->f110)->Init((Actor*)o, 0x28000, 0x50000, 0x204004, 0);
            ((WithMeshClsn*)&o->f144)->Init((Actor*)o, 0x32000, 0x32000, 0, 0);
        }
    }

    o->f100 = 0;
    o->f3b6 = 0;
    *(short*)((char*)o + 0x300 + 0xb4) = 0;
    o->f3a8 = o->f5c;
    o->f3ac = o->f60;
    o->f3b0 = o->f64;

    if ((unsigned int)(o->f3c8 - 1) <= 1)
    {
        o->f9c = -0x3000;
        o->fa0 = -0x50000;
    }

    return 1;
}
