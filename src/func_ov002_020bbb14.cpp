//cpp
#include "types.h"
// @symbol func_ov002_020bbb14
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* c);
extern void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* p);
extern int _ZNK12WithMeshClsn12TouchesWaterEv(void* p);
extern void* _ZN5Actor10FindWithIDEj(unsigned id);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* thiz, void* v, unsigned a, int b, unsigned c, unsigned d, unsigned e);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern int _ZN16MeshColliderBase9IsEnabledEv(void* p);
extern void _ZN16MeshColliderBase7DisableEv(void* p);
extern int _ZN5Actor13DistToCPlayerEv(void* self);
extern void func_ov002_020bae9c(char* c);
}

struct Obj {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
};

extern "C" void func_ov002_020bbb14(char* self);

void func_ov002_020bbb14(char* self)
{
    int b;
    struct Vector3 vec;
    void* found;
    unsigned id;

    {
        s16* pa = (s16*)((long long)(int)(self + 0x8c));
        *pa = *pa + 0x2000;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
    WithMeshClsn_UpdateContinuous_Veneer(self + 0x3c8);

    if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x3c8) != 0 ||
        _ZNK12WithMeshClsn8IsOnWallEv(self + 0x3c8) != 0 ||
        _ZNK12WithMeshClsn12TouchesWaterEv(self + 0x3c8) != 0) {
        ((Obj*)self)->v31();
        return;
    }

    id = *(unsigned*)(self + 0x344);
    if (id != 0) {
        found = _ZN5Actor10FindWithIDEj(id);
        if (found != 0) {
            if (found != *(void**)(self + 0x5a0)) {
                b = *(u16*)((char*)found + 0xc);
                b = b == 0xbf;
                if (b) {
                    vec.x = *(int*)(self + 0x5c);
                    vec.y = *(int*)(self + 0x60);
                    vec.z = *(int*)(self + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(found, &vec, 1, 0xc000, 1, 0, 1);
                }
            }
        }
    }

    _Z14ApproachLinearRiii((int*)(self + 0x98), 0, 0x555);

    if (_ZN16MeshColliderBase9IsEnabledEv(self + 0x124) != 0) {
        _ZN16MeshColliderBase7DisableEv(self + 0x124);
    }

    b = *(int*)(self + 0xb0) & 8;
    b = b != 0;
    if (b) {
        if (_ZN5Actor13DistToCPlayerEv(self) > 0x7d0000) {
            func_ov002_020bae9c(self);
        }
    }
}
