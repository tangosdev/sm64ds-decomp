//cpp
#include "types.h"
typedef void* (*Vfn)();

struct Animation { void Advance(); };
struct CylinderClsn { void Clear(); void Update(); };
struct WithMeshClsn { int IsOnGround() const; };
struct Vector3;

extern "C" void func_ov030_02111a00(void* c);
extern "C" void func_ov030_02111f6c(char* c, WithMeshClsn* w);
extern "C" void func_ov030_02111bc4(void* c);
extern "C" int Vec3_Dist(const Vector3* a, const Vector3* b);
extern "C" void func_ov030_021141a8(void* c, int x);
extern "C" void* data_02099368[];

struct Actor {
    void UpdatePos(CylinderClsn*);
};

typedef struct { int a, b; } P2;
struct ClsnResult {
    Vfn* vtb;
    P2 v;
    int a2, a3, a4;
    u16 h0, h1;
    int t0, t1, t2;
    int GetClsnID() const;
};
extern "C" ClsnResult* func_0203567c(WithMeshClsn* w);
extern "C" int func_02037f44(ClsnResult* r);
extern "C" void _ZN10ClsnResultD1Ev(ClsnResult* r);

extern "C" int func_ov030_02112400(char* c)
{
    func_ov030_02111a00(c);
    ((Animation*)(c + 0x124))->Advance();
    ((Actor*)c)->UpdatePos((CylinderClsn*)(c + 0x160));
    func_ov030_02111f6c(c, (WithMeshClsn*)(c + 0x194));
    func_ov030_02111bc4(c);
    ((CylinderClsn*)(c + 0x160))->Clear();
    ((CylinderClsn*)(c + 0x160))->Update();

    int b = (int)(*(u16*)(c + 0xc) == 0x10c);
    if (b != 0) {
        if (Vec3_Dist((Vector3*)(c + 0x380), (Vector3*)(c + 0x5c)) < 0x514000) {
            if (*(int*)(c + 0x60) > *(int*)(c + 0x384) - 0x12c000) {
                func_ov030_021141a8(c, 1);
            }
        }
    } else {
        if (((WithMeshClsn*)(c + 0x194))->IsOnGround()) {
            char* r = (char*)func_0203567c((WithMeshClsn*)(c + 0x194));
            ClsnResult res;
            int* d = (int*)&res.v;
            *(double*)d = *(double*)(r + 4);
            d[2] = *(int*)(r + 0xc);
            d[3] = *(int*)(r + 0x10);
            d[4] = *(int*)(r + 0x14);
            res.vtb = (Vfn*)data_02099368;
            res.h0 = *(u16*)(r + 0x18);
            res.h1 = *(u16*)(r + 0x1a);
            res.t0 = *(int*)(r + 0x1c);
            res.t1 = *(int*)(r + 0x20);
            res.t2 = *(int*)(r + 0x24);
            if (res.GetClsnID() == -1 || func_02037f44(&res) == 0) {
                func_ov030_021141a8(c, 0);
            }
            _ZN10ClsnResultD1Ev(&res);
        }
    }
    return 1;
}
