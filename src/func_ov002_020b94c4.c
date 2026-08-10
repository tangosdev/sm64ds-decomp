typedef unsigned int u32;
typedef short s16;
typedef long long s64;
typedef int Fix12;

typedef struct Vector3 { int x, y, z; } Vector3;
struct RG { char a[0x14]; int detect[15]; };

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, Fix12 c, Fix12 d, Fix12 e, const void* f, void* g);
extern void func_0203568c(int *p, int v);
extern void func_02035684(int *p, int v);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* c);
extern void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern int _ZNK12WithMeshClsn12TouchesWaterEv(void* self);
extern void _ZN13RaycastGroundC1Ev(struct RG*);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RG*, const Vector3*, void*);
extern int _ZN13RaycastGround10DetectClsnEv(struct RG*);
extern void _ZN5Actor13SmallPoofDustEv(void* self);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern void _ZN13RaycastGroundD1Ev(struct RG*);
extern void _ZN12WithMeshClsn18StopDetectingWaterEv(void* self);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern int func_0200fccc(char* s, int r1);
extern void func_ov002_020b9704(void* c, int i);

void func_ov002_020b94c4(char* c)
{
    struct RG rg;
    Vector3 pos;
    int a;
    int mag;
    int y;
    s16 delta;
    s16 X;
    int gy;
    int diff;

    *(u32*)(c + 0x3c4) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x3c4), 0x103, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64), 0, 0);

    a = *(int*)(c + 0xa8);
    mag = (a < 0) ? -a : a;
    y = (int)(((s64)mag * 0x120000 + 0x800) >> 12);
    X = (s16)(y / 4096);
    delta = 0x250;
    if (X > 0)
        delta += X;
    if (a > 0)
        *(s16*)(c + 0x8e) += delta;
    else
        *(s16*)(c + 0x8e) -= delta;

    func_0203568c((int*)(c + 0x200), 0x3c000);
    func_02035684((int*)(c + 0x200), 0x3c000);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x1cc);
    WithMeshClsn_UpdateContinuous_Veneer((void*)(c + 0x200));

    if (_ZNK12WithMeshClsn12TouchesWaterEv((void*)(c + 0x200))) {
        pos.x = *(int*)(c + 0x5c);
        pos.y = *(int*)(c + 0x60);
        pos.z = *(int*)(c + 0x64);
        _ZN13RaycastGroundC1Ev(&rg);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, 0);
        if (_ZN13RaycastGround10DetectClsnEv(&rg)) {
            gy = rg.detect[12];
            pos.y = gy;
            diff = *(int*)(c + 0x60) - gy;
            if (diff < 0)
                diff = -diff;
            if (diff > 0x64000) {
                _ZN5Actor13SmallPoofDustEv(c);
                _ZN9ActorBase18MarkForDestructionEv(c);
                _ZN13RaycastGroundD1Ev(&rg);
                return;
            }
            _ZN12WithMeshClsn18StopDetectingWaterEv((void*)(c + 0x200));
        } else {
            _ZN5Actor13SmallPoofDustEv(c);
            _ZN9ActorBase18MarkForDestructionEv(c);
            _ZN13RaycastGroundD1Ev(&rg);
            return;
        }
        _ZN13RaycastGroundD1Ev(&rg);
        return;
    }

    if (!_ZNK12WithMeshClsn10IsOnGroundEv((void*)(c + 0x200)))
        return;
    func_0200fccc(c, 1);
    func_ov002_020b9704(c, 1);
}
