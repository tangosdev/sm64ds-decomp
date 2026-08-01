//cpp
// @symbol func_ov092_021311b0
/* recovered: shared common types */
#include "common.h"

typedef short s16;
#define LA(p) (((long long)(int)(p)))
extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
void _ZN5Actor14TriplePoofDustEv(void *self);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *pos);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
int func_02037e38(void *p);
int func_02037e84(void *p);
void func_0203568c(void *p, int v);
void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *self, Vector3 *pos, int fix);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int b, int cc, int d);
void WithMeshClsn_UpdateContinuous_Veneer(void *self);
void func_02012694(int a, void *pos);
}
extern "C" void func_ov092_021311b0(void *thiz)
{
    char *c = (char *)thiz;
    volatile int saved[3];
    Vector3 v1;
    Vector3 v2;
    char *fr;

    {
        s16 *p8c = (s16 *)LA(c + 0x8c);
        s16 *p8e = (s16 *)LA(c + 0x8e);
        s16 *p90 = (s16 *)LA(c + 0x90);
        *p8c += ((s16 *)(c + 0x400))[0xe0 / 2];
        *p8e += ((s16 *)(c + 0x400))[0xe2 / 2];
        *p90 += ((s16 *)(c + 0x400))[0xe4 / 2];
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

    saved[0] = *(int *)(c + 0x5c);
    saved[1] = *(int *)(c + 0x60);
    saved[2] = *(int *)(c + 0x64);
    WithMeshClsn_UpdateContinuous_Veneer(c + 0x324);

    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x324) != 0) {
        _ZN5Actor14TriplePoofDustEv(c);
        _ZN9ActorBase18MarkForDestructionEv(c);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x41, c + 0x74);
        return;
    }

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x324) == 0)
        return;

    *(int *)(c + 0x55c) = *(int *)(c + 0x60);
    fr = (char *)_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x324);

    if (func_02037e38(fr + 4) == 9) {
        int z;
        *(int *)(c + 0x568) = 8;
        z = 0;
        *(int *)(c + 0x98) = z;
        *(int *)(c + 0xa8) = z;
        func_02012694(0x178, c + 0x74);
    } else if (func_02037e84(fr + 4) == 8) {
        char *o;
        int *p;
        *(int *)(c + 0x568) = 7;
        func_0203568c(c + 0x324, 0x78000);
        o = *(char **)(c + 0x320);
        p = (int *)LA(o + 0x5c);
        v1.x = p[0];
        v1.y = p[1];
        v1.z = p[2];
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v1, 0xff8000);
        *(int *)(c + 0x5c) = saved[0];
        *(int *)(c + 0x60) = saved[1];
        *(int *)(c + 0x64) = saved[2];
        func_02012694(0x178, c + 0x74);
    } else {
        char *o;
        int *p;
        _ZN5Actor14TriplePoofDustEv(c);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfa, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
        _ZN9ActorBase18MarkForDestructionEv(c);
        o = *(char **)(c + 0x320);
        p = (int *)LA(o + 0x5c);
        v2.x = p[0];
        v2.y = p[1];
        v2.z = p[2];
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v2, 0xff8000);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x41, c + 0x74);
    }
    *(int *)(c + 0x320) = 0;
}
