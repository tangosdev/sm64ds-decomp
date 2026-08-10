//cpp
#include "types.h"
// @symbol func_ov026_02111330
// recovered name: daObjWlPolelift_c_Behavior
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daObjWlPolelift_c::Behavior - recovered from vtable slot identity */
extern "C" {
    unsigned short DecIfAbove0_Short(unsigned short* p);
    void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, Vector3* out, unsigned int idx);
    void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
    int LenVec3(Vector3* v);
    int _ZN4cstd4fdivEii(int a, int b);
    void Vec3_MulScalar(Vector3* out, Vector3* a, int s);
    void SubVec3(Vector3* a, Vector3* b, Vector3* c);
    int _ZNK7PathPtr5LoopsEv(void* self);
    void _ZN12CylinderClsn5ClearEv(void* self);
    void _ZN13RaycastGroundC1Ev(void* self);
    void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, const Vector3* pos, void* actor);
    int _ZN13RaycastGround10DetectClsnEv(void* self);
    void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, const Vector3* v);
    void _ZN12CylinderClsn6UpdateEv(void* self);
    void func_ov026_021112a4(void* t);
    void func_ov026_02111234(void* c);
    void _ZN13RaycastGroundD1Ev(void* self);
}


struct RaycastGround { char pad[0x44]; s32 hitY; char pad2[8]; };


extern "C" int func_ov026_02111330(void* self)
{
    u8* c = (u8*)self;
    Vector3 a, b, sub, rp, mul, rel;
    RaycastGround rg;
    int moved;
    int idx;
    int spd, cur, len, s, flag;

    if (DecIfAbove0_Short((unsigned short*)(c + 0x184)) == 0) {
        spd = *(s32*)(c + 0x180);
        cur = *(s32*)(c + 0x170);
        idx = cur - spd;
        moved = 0;
        if (spd > 0) {
            if (idx < 0) idx = *(s32*)(c + 0x16c) - 1;
        } else {
            if (idx >= *(s32*)(c + 0x16c)) idx = moved;
        }

        _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x164, &a, cur);
        _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x164, &b, idx);
        Vec3_Sub(&sub, (Vector3*)(c + 0x5c), &a);
        len = LenVec3(&sub);
        if (len == 0 || len <= 0xa000) {
            *(s32*)(c + 0x5c) = a.x;
            moved = 1;
            *(s32*)(c + 0x60) = a.y;
            *(s32*)(c + 0x64) = a.z;
        } else {
            s = _ZN4cstd4fdivEii(0xa000, len);
            Vec3_MulScalar(&mul, &sub, s);
            SubVec3((Vector3*)(c + 0x5c), &mul, (Vector3*)(c + 0x5c));
        }

        if (moved) {
            *(volatile s32*)((s32)c + 0x170) += *(s32*)(c + 0x180);
            if (*(s32*)(c + 0x170) < 0) {
                if (_ZNK7PathPtr5LoopsEv(c + 0x164) != 0) {
                    *(s32*)(c + 0x170) = *(s32*)(c + 0x16c) - 1;
                } else {
                    *(s16*)(c + 0x184) = 0x3c;
                    *(s32*)(c + 0x180) = 1;
                    *(volatile s32*)((s32)(c + 0x170)) += *(s32*)(c + 0x180) << 1;
                }
            }
            if (*(s32*)(c + 0x170) >= *(s32*)(c + 0x16c)) {
                if (_ZNK7PathPtr5LoopsEv(c + 0x164) != 0) {
                    *(s32*)(c + 0x170) = 0;
                } else {
                    *(s16*)(c + 0x184) = 0x3c;
                    *(s32*)(c + 0x180) = -1;
                    *(volatile s32*)((s32)(c + 0x170)) += *(s32*)(c + 0x180) << 1;
                }
            }
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x124);
    flag = (*(s32*)(c + 0xb0) & 8) != 0;
    if (!flag) {
        rp = *(Vector3*)(c + 0x5c);
        rp.y -= 0x14000;
        _ZN13RaycastGroundC1Ev(&rg);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &rp, 0);
        *(s32*)(c + 0x1e0) = rp.y;
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            *(s32*)(c + 0x1e0) = rg.hitY;
        }
        rel = data_ov026_02113a9c;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x124, &rel);
        _ZN12CylinderClsn6UpdateEv(c + 0x124);
        func_ov026_021112a4(self);
        func_ov026_02111234(self);
        _ZN13RaycastGroundD1Ev(&rg);
    }
    return 1;
}
