typedef int Fix12;
struct Vector3 { int x, y, z; };
struct RG { char pad[0x44]; int f44; char pad2[8]; };
struct Blk { int w[12]; };

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void* self, void* actor, const struct Vector3* v, Fix12 a, int b, unsigned int c, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void* self, void* actor, Fix12 a, int b, void* v, int c);
extern void func_ov070_02121880(void* c, int n);
extern void _ZN13RaycastGroundC1Ev(struct RG* rg);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RG* rg, const struct Vector3* v, void* a);
extern int _ZN13RaycastGround10DetectClsnEv(struct RG* rg);
extern void func_ov070_02121310(void* c);
extern void _ZN13RaycastGroundD1Ev(struct RG* rg);

extern void* data_ov070_02123698;
extern struct Blk data_02082128;

int func_ov070_02121914(char* self)
{
    struct RG rg;
    struct Vector3 v;
    void* bmd;
    int t;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov070_02123698);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, 1);
    if (!_ZN11ShadowModel12InitCylinderEv(self + 0x138))
        return 0;

    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        self + 0x160, self, &v, 0x32000, 0x64000, 0x200002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        self + 0x1a0, self, 0x32000, 0x32000, 0, 0);

    *(int*)(self + 0x9c) = 0;
    *(int*)(self + 0xa0) = 0;
    func_ov070_02121880(self, 0);

    *(int*)(self + 0x80) = 0x1000;
    *(int*)(self + 0x84) = 0x1000;
    *(int*)(self + 0x88) = 0x1000;
    *(struct Blk*)(self + 0x35c) = data_02082128;

    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, (struct Vector3*)(self + 0x5c), self);
    if (_ZN13RaycastGround10DetectClsnEv(&rg))
        t = (*(int*)(self + 0x60) - rg.f44) + 0x1e000;
    else
        t = 0x1f4000;
    *(int*)(self + 0x3a8) = t;
    func_ov070_02121310(self);
    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
