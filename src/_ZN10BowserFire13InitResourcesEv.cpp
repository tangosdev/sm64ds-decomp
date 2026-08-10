//cpp
// @symbol _ZN10BowserFire13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * BowserFire holds no file references of its own -- Bowser loads and frees the
 * whole fight -- so this sets up collision and state rather than resources.
 *
 * All three shadow declarations are gone:
 *   - `struct Vector3 { int x, y, z; }`   -> the real types.h Vector3.
 *   - `struct RaycastGround { ... }`      -> the real RaycastGround.h.
 *   - `struct Actor { }`                  -> the real Actor.h.
 * ...along with the magic offsets on `char *c`, now named BowserFire members.
 *
 * The RaycastGround one is the interesting fix. The stand-in declared
 * `int floor[12]` at 0x14 and then read `floor[12]` -- one PAST its own bound,
 * so the index was a magic offset in disguise: 0x14 + 12*4 = 0x44. The real
 * header names that field `clsnY` and documents it as the search seed on entry
 * and the hit on exit, which is exactly how it is used here.
 *
 * The `Actor` one was NOT obviously safe and was measured rather than assumed.
 * It types the pointer-to-member dispatch through data_ov060_0211af74, and a
 * pointer to member of a POLYMORPHIC class need not share a representation
 * with one of an empty class. Under the pin it does: swapping the empty
 * stand-in for the real Actor is byte-identical.
 *
 * The `|= 1` at 0x2e8 was briefly named as a BowserFire field of its own. It
 * is not one. 0x2d0 + 0x18 lands inside mMovingCylinderClsn, and
 * CylinderClsn::flags is at 0x18, documented as "bit 0 makes Update bail" --
 * which is precisely what setting bit 0 does, and precisely what this branch
 * wants when unk_35c is zero. Same mistake, and same correction, as Player's
 * `mBodyClsnFlags`.
 *
 * The doubled write to pos.y is the ROM's own shape and is kept verbatim: the
 * seed is read into a local, stored, then overwritten with seed + 0x32000.
 */
#include "BowserFire.h"
#include "RaycastGround.h"
#include "Actor.h"

typedef void (Actor::*ActorFn)();

extern "C" {
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
extern void _ZN13RaycastGroundC1Ev(RaycastGround *self);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround *self);
extern void _ZN13RaycastGroundD1Ev(RaycastGround *self);
}

extern ActorFn data_ov060_0211af74[];

int BowserFire::InitResources()
{
    RaycastGround rc;
    Vector3 pos;

    if (_ZN11ShadowModel12InitCylinderEv(&this->mShadowModel) == 0)
        return 0;

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        &this->mMovingCylinderClsn, this, 0x28000, 0x50000, 0x200002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        &this->mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);

    this->unk_09c = -0x4000;
    this->unk_0a0 = -0x1e000;
    this->unk_35c = this->mParam & 7;
    this->unk_374 = 0;
    if (this->unk_35c == 0)
        this->unk_379 = 0;
    else
        this->unk_379 = 1;
    this->unk_36c = 0;
    this->unk_378 = ((unsigned int)this->mParam >> 4) & 3;
    if (this->unk_35c == 0)
        this->mMovingCylinderClsn.flags |= 1;
    this->unk_360 = 0x2000;
    this->unk_380 = 0;
    this->unk_37c = this->unk_380;
    this->unk_2cc = 0;

    _ZN13RaycastGroundC1Ev(&rc);
    {
        int p60;
        pos.x = this->mPosX;
        p60 = this->mPosY;
        pos.y = p60;
        pos.z = this->mPosZ;
        pos.y = p60 + 0x32000;
    }
    rc.SetObjAndPos(pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc))
        this->unk_364 = rc.clsnY;
    else
        this->unk_364 = this->mPosY;

    (((Actor *)this)->*data_ov060_0211af74[this->unk_35c])();

    this->unk_384 = 0;
    this->unk_388 = 0;
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
