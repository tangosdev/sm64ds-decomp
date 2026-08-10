//cpp
#include "types.h"
// @symbol _ZN5Unagi8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Unagi.h"
struct C;
typedef void (C::*PMF)();

extern "C" u16 DecIfAbove0_Short(u16* p);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cc);
extern "C" void* _ZN5Actor10FindWithIDEj(unsigned id);
extern "C" void* _ZN5Actor13ClosestPlayerEv(void* self);
extern "C" int Vec3_Dist(const void* a, const void* b);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern "C" void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const Vector3& pos, const Vector3_16* rot, int e, int f);
extern "C" void _ZN12CylinderClsn5ClearEv(void* self);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* self);
extern "C" void _ZN14BlendModelAnim7AdvanceEv(void* self);

struct Disp { int unused[2]; PMF pmf; };

extern "C" unsigned char data_0209f220;
extern void* data_ov016_02114dbc;

#define LA(p) ((void*)(unsigned long)((unsigned long)(p)))

int Unagi::Behavior()
{
    void* found;
    unsigned id;

    DecIfAbove0_Short((u16*)((char*)&unk_100));
    {
        Disp* d = *(Disp**)((char*)&unk_34c);
        if (d->pmf) {
            (((C*)((char*)this))->*(d->pmf))();
        }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), ((char*)this) + 0x110);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov016_02111c40(((char*)this));

    id = *(unsigned*)((char*)&mStarUniqueID);
    if (id != 0) {
        found = _ZN5Actor10FindWithIDEj(id);
        if (found == 0)
            goto clear_id;
        if (mVariant == 1) {
            void* cp = _ZN5Actor13ClosestPlayerEv(((char*)this));
            if (cp != 0) {
                int off43c = 0x43c;
                if (Vec3_Dist(((char*)this) + off43c, (char*)cp + 0x5c) < 0xfa000) {
                    _ZN9ActorBase18MarkForDestructionEv(found);
                    {
                        int param = mAreaId;
                        int m1 = -1;
                        int off = 0x43c;
                        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                            0xb2, unk_414 | 0x40,
                            *(Vector3*)(((char*)this) + off), (Vector3_16*)0,
                            param, m1);
                    }
                    mStarUniqueID = 0;
                    goto after_first;
                }
            }
        }
        goto after_first;
    clear_id:
        mStarUniqueID = 0;
    after_first:
        if (mStarUniqueID != 0) {

        int x = unk_43c;
        int off = 0x416;
        *(int*)((char*)found + 0x5c) = x;
        short* r = (short*)LA(((char*)this) + off);
        *(int*)((char*)found + 0x60) = unk_440;
        *(int*)((char*)found + 0x64) = unk_444;
        *r += 0x1000;

        }
    }

    func_ov016_02111284(((char*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos1);
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos1);
    if (data_0209f220 == 1 && *(void**)((char*)&unk_34c) != &data_ov016_02114dbc) {
        _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos2);
        _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos2);
    }
    _ZN14BlendModelAnim7AdvanceEv((char*)&mBlendModelAnim);
    return 1;
}
