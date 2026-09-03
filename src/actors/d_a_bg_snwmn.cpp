//cpp
/* PROMOTED translation unit -- ov072/daBgSnwmn_c (8 function(s)).
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object, the way the
 * cartridge's own build did. It licenses the contiguous .text run
 * 0x02120824..0x02120c00 in ov072 (config/tu_manifest.d/ov072/daBgSnwmn_c.json,
 * config/arm9/overlays/ov072/delinks.txt).
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm emits one .text section per function
 * in the reverse of source order, so the highest-address ROM function is
 * written FIRST and the lowest last. Do not reorder these blocks.
 *
 * The two destructor variants are `// @symbol` mangled bodies rather than a
 * real `~daBgSnwmn_c()` member, and the vptr stores are spelled
 * `&_ZTV11daBgSnwmn_c[2]` rather than the bare symbol. Both choices are
 * forced; the measurements behind them are written out at the D0 body below,
 * and the matching header note is in include/daBgSnwmn_c.h.
 *
 * The functions, in ROM address order:
 *   [0] 0x02120824  _ZN11daBgSnwmn_cD1Ev
 *   [1] 0x02120874  _ZN11daBgSnwmn_cD0Ev
 *   [2] 0x021208d8  func_ov072_021208d8
 *   [3] 0x02120980  daBgSnwmn_c::CleanupResources
 *   [4] 0x021209bc  daBgSnwmn_c::OnPendingDestroy
 *   [5] 0x021209c0  daBgSnwmn_c::Render
 *   [6] 0x02120a08  daBgSnwmn_c::Behavior
 *   [7] 0x02120a44  daBgSnwmn_c::InitResources
 */

/* Includes. daBgSnwmn_c.h carries the class; the decl_* headers supply the
 * Actor/Model/ShadowModel declarations this file's bodies reach for. */
#include "daBgSnwmn_c.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"

/* Externs for symbols this TU calls but does not own, carried from the legacy
 * one-function sources. The BMD_File/BTP_File handles genuinely have no
 * project header and stay as raw int[]; several of the others (dCcAcPos_c,
 * TextureSequence, SharedFilePtr) DO have headers in include/ that this file
 * already pulls in via daBgSnwmn_c.h, so those externs duplicate a real
 * declaration and should be replaced by it -- deliberately left for a
 * follow-up, since each swap has to be re-proved against the ROM. */
/* shadow struct 'Matrix4x3' */
struct Matrix4x3;

/* shadow struct 'ShadowModel' */
struct ShadowModel;

/* shadow struct 'Sub' */
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };

extern "C" {
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j( void *thiz, struct ShadowModel *sm, struct Matrix4x3 *mtx, int a, int b, unsigned int n);
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov072_02122c48[];
extern "C" int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
extern const Vector3 data_ov072_02122c70;
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, void *pos, void *ang, int a, int b);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, u32 u);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *act, const Vector3 &pos, int f1, int f2, u32 u1, u32 u2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN11daBgSnwmn_c13InitResourcesEv, 0x02120a44, size 0x1bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daBgSnwmn_c13InitResourcesEv
/* recovered: named members + real C++ method */
/* daBgSnwmn_c::InitResources() -- vtable slot 0. If level 0xa's star 5 is NOT
 * yet collected, it spawns actor 0x111 in its place and marks itself for
 * destruction -- but does not return, so the rest of the body still runs.
 * It loads two models and the texture sequence, sets up the cylinder shadow and
 * collision, then drops the snowman onto the ground: a throwaway dBgCh_Gnd
 * probe 0x14000 above the spawn point finds the floor, and mPosY is set from
 * the hit (or the probe height) and raised 0xc3000. */
s32 daBgSnwmn_c::InitResources()
{
    Vector3 pos;
    void *m;

    if (IsStarCollectedInLevel(0xa, 5) == 0)
    {
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x111, 0, &mPosX, &mAngleX, mAreaId, -1);
        _ZN7fBase_c18MarkForDestructionEv(this);
    }

    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c48);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel1, m, 1, 1);
    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c40);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, m, 1, 1);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov072_02122c50);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void *)data_ov072_02122c48[1], (void *)data_ov072_02122c50[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTexSeq, (void *)data_ov072_02122c50[1], 0, 0x1000, 0);

    if (_ZN11ShadowModel12InitCylinderEv(&mShadow) == 0)
        return 0;

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mCylClsn, this, data_ov072_02122c70, 0xc3000, 0x17c000, 0x800004, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mPosY += 0xc3000;
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mScaleX = 0x1800;
    mScaleY = 0x1800;
    mScaleZ = 0x1800;
    func_ov072_021208d8(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN11daBgSnwmn_c8BehaviorEv, 0x02120a08, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daBgSnwmn_c8BehaviorEv
/* daBgSnwmn_c::Behavior() -- vtable slot 6. See include/daBgSnwmn_c.h. All
 * four calls reach into the typed mTexSeq/mCylClsn members. */
s32 daBgSnwmn_c::Behavior()
{
    mTexSeq.Advance();
    mCylClsn.SetPosRelativeToActor(data_ov072_02122c70);
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN11daBgSnwmn_c6RenderEv, 0x021209c0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daBgSnwmn_c6RenderEv
/* daBgSnwmn_c::Render() -- vtable slot 9. See include/daBgSnwmn_c.h. `Sub`
 * is a local stand-in for Model's own 6-slot vtable shape (mModel1/2's
 * shared g5, whichever ModelBase virtual that is) -- kept as the previous
 * recovery pass wrote it, not the typed Model member, since it calls
 * through a vtable slot rather than a named method. Real C++ method now. */
s32 daBgSnwmn_c::Render(){
  _ZN15TextureSequence6UpdateER15ModelComponents((char*)this+0x174, (char*)this+0xdc);
  ((Sub*)((char*)this+0xd4))->g5((char*)this+0x80);
  ((Sub*)((char*)this+0x124))->g5((char*)this+0x80);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN11daBgSnwmn_c16OnPendingDestroyEv, 0x021209bc, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daBgSnwmn_c16OnPendingDestroyEv
/* recovered: real C++ method */
/* daBgSnwmn_c::OnPendingDestroy() -- vtable slot 12. Empty override. */
void daBgSnwmn_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN11daBgSnwmn_c16CleanupResourcesEv, 0x02120980, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daBgSnwmn_c16CleanupResourcesEv
/* recovered: real C++ method */
/* daBgSnwmn_c::CleanupResources() -- vtable slot 3. Drops the three shared
 * file handles InitResources took: two models and the texture sequence. */
s32 daBgSnwmn_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c48);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c40);
    _ZN13SharedFilePtr7ReleaseEv(data_ov072_02122c50);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov072_021208d8, 0x021208d8, size 0xa8 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov072_021208d8(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;

    Matrix4x3_FromRotationY(c + 0xf0, *(short *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = (*(int *)(c + 0x60) + 0x13b000) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;

    Matrix4x3_FromRotationY(c + 0x140, *(short *)(c + 0x8e));
    *(int *)(c + 0x164) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x168) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x16c) = *(int *)(c + 0x64) >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (struct ShadowModel *)(c + 0x188), (struct Matrix4x3 *)(c + 0x140), 0xe6000, 0x12c000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11daBgSnwmn_cD0Ev, 0x02120874, size 0x64 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* mangled body: C linkage so the name is emitted verbatim */
// @symbol _ZN11daBgSnwmn_cD0Ev
/* recovered: named members + shared header, vtable identified
 *
 * D0 is the DELETING destructor: store this class's vtable over the one the
 * base constructor left, destroy the five typed members in reverse
 * declaration order, run the dActor_c base-subobject destructor, then return
 * the object to the game heap.  Its first 0x40 bytes are the same SEQUENCE of
 * operations as D1's, but not the same bytes: the literal-pool displacement
 * and every bl displacement differ, and from 0x40 on the two diverge outright
 * (D1 pops and returns; D0 loads the heap pointer and calls
 * Memory::Deallocate).
 *
 * Written as a mangled body rather than as a real `~daBgSnwmn_c()` member.  A
 * real member definition makes mwccarm emit the D2/D1/D0 triple as ONE group
 * in the order D0-then-D1, while the cartridge keeps D1 (0x02120824) BELOW D0
 * (0x02120874); the whole-range linkcheck then refuses with `licensed .text
 * functions are not emitted in ROM address order`, and the group also carries
 * an unhomed D2.  tubuild's own `functions_occur_in_expected_order` check
 * does NOT predict this: it reads PASS in the manifest, because the ROM order
 * is fine -- it is mwccarm's emission order that is not.
 *
 * THE VPTR STORE IS SPELLED &_ZTV[2], AND HAS TO BE.  symbols.txt records
 * _ZTV11daBgSnwmn_c at ov072:0x02122978, which IS the vtable's address point --
 * the cartridge's literal pool word at 0x02120870 holds exactly that.  But this
 * TU DEFINES the vtable, and mwccarm's own _ZTV symbol addresses the vtable
 * OBJECT's start, 8 bytes lower (offset-to-top, then _ZTI).  Production
 * isolation externalises the locally-defined symbol and subtracts that 8-byte
 * preamble, so it requires an addend of at least 8: with `(int)_ZTV11daBgSnwmn_c`
 * -- addend 0 -- tools/objisolate.py refuses both destructors with
 * `_ZTV11daBgSnwmn_c: unexpected reloc type=2 addend=0`.  Writing
 * `&_ZTV11daBgSnwmn_c[2]` gives addend 8, which the subtraction turns back into
 * the ROM's 0x02122978.  A TU that only IMPORTS a vtable takes addend 0 instead;
 * the spelling follows from who defines it, not from the class.
 */
extern int _ZTV11daBgSnwmn_c[];
extern void *data_020a0eac;   /* GAME_HEAP_PTR */
/* The member destructors this body calls, by their exact ROM names.
   Model/ShadowModel/dActor_c already have decl_*.h headers (included above);
   dCcAcPos_c's and TextureSequence's D1 are not declared anywhere yet, so they
   are spelled here. */
extern void _ZN10dCcAcPos_cD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
int *_ZN11daBgSnwmn_cD0Ev(int *t)
{
    t[0] = (int)&_ZTV11daBgSnwmn_c[2];
    _ZN10dCcAcPos_cD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN15TextureSequenceD1Ev((char *)t + 0x174);
    _ZN5ModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN11daBgSnwmn_cD1Ev, 0x02120824, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* mangled body: C linkage so the name is emitted verbatim */
// @symbol _ZN11daBgSnwmn_cD1Ev
/* recovered: named members + shared header, vtable identified
 *
 * D1 is the COMPLETE-OBJECT destructor: the same body as D0 without the
 * deallocation.  See the D0 block above for why both are written as mangled
 * bodies instead of one real `~daBgSnwmn_c()` member.
 */
extern int _ZTV11daBgSnwmn_c[];
int *_ZN11daBgSnwmn_cD1Ev(int *t)
{
    t[0] = (int)&_ZTV11daBgSnwmn_c[2];
    _ZN10dCcAcPos_cD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN15TextureSequenceD1Ev((char *)t + 0x174);
    _ZN5ModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
}
