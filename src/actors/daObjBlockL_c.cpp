//cpp
/* Big brick blocks: one class, six actor IDs (0xf, 0x10, 0x11, 0x12,
 * 0x13, 0x2e) -- item blocks, the star block, the switch-activated
 * block. Kill is a switchboard on actorID: coins, stars, particles,
 * then destroy, except the 0x13 block which the event bit re-enables.
 * Factories live outside this TU.
 *
 * Leftover: Particle::System::NewSimple, dActor_c::SpawnCoins and
 *   dBgActor_c::IsClsnInRangeOnScreen stay mangled (Fix12<int> by
 *   value, wall 6az).
 * Leftover: dBgW_KcMbg::SetFile keeps its mangled spelling (by-value
 *   Fix12<int> parameters, wall 6az).
 * Leftover: the BigBrickBlockFileRow/SharedFileRow/ClpsRow tables
 *   stay file-local: no shared header owns those 0xc-stride rows yet.
 * Leftover: Event::GetBit's conflicting declarations resolve to the
 *   first spelling (manifest notes both alternates).
 * Leftover: linkedActor + 0x3b0 / + 0xd6 are unnamed per-actor bytes
 *   this helper clears/sets.
 * Leftover: data_0209f2f8 reads as the level ID and data_0209f220 as
 *   a star flag (same pattern in the daObjBk_Botaosi_c TU); neither has
 *   a shared home.
 * Leftover: IsStarCollectedInCurLevel is a shared coined helper;
 *   naming belongs at its definition.
 */

#include "daObjBlockL_c.h"
#include "Player.h"
#include "dActor_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "daObjSwitch_c.h"
#include "Model.h"

/* File-local resource rows (see the header note above). */
struct BigBrickBlockFileRow {
    SharedFilePtr* file;
    u8 pad[8];
};

/* shadow struct 'SharedFileRow' */
struct SharedFileRow {
    SharedFilePtr *file;
    u8 pad[8];
};

/* shadow struct 'ClpsRow' */
struct ClpsRow {
    CLPS_Block *block;
    u8 pad[8];
};

extern "C" {
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_( u32 id, Fix12i x, Fix12i y, Fix12i z);
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs( void *self, const Vector3 &pos, u32 count, s32 speed, s16 delay);
extern BigBrickBlockFileRow data_ov002_02108ab0[];
extern BigBrickBlockFileRow data_ov002_02108ab4[];
extern "C" int _ZN5Event6GetBitEj(unsigned int bit);
extern Vector3 data_ov002_021089e0;
extern "C" void func_020393a4(dBgW_KcMbg *p, int v);
extern "C" void func_02039394(dBgW_KcMbg *p, int v);
extern "C" void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern ClpsRow data_ov002_02108ab8[];         /* CLPS blocks,       0xc stride */
extern s8 data_0209f2f8;              /* current level */
extern u8 data_0209f220;
int IsStarCollectedInCurLevel(int star);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mat, int scale, s16 angY,
CLPS_Block *clps);
}

/* Emission order is ROM order: functions must stay ROM-ascending.
 * Do not reorder. */
#pragma defer_codegen off

/* ROM ordinals 0 and 1 -- _ZN13daObjBlockL_cD1Ev 0x020b35a0 size 0x44,
                          _ZN13daObjBlockL_cD0Ev 0x020b35e4 size 0x58 */
// @symbol _ZN13daObjBlockL_cD1Ev
// @symbol _ZN13daObjBlockL_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * ONE definition, TWO ROM functions. Two vtable stores and three destructor
 * calls, every one a consequence of `struct daObjBlockL_c : dBgActor_c`: its
 * own vptr, then dBgActor_c's -- inlined, because dBgActor_c's destructor is
 * defined in its class body -- then dBgActor_c's Model and dBgW_KcMbg, then
 * dActor_c. This class adds no member with a destructor of its own. D0
 * additionally returns the object to the actor heap through the inline
 * operator delete, which is why nothing below mentions a heap. With deferred
 * code generation off the two variants land in the cartridge's order, D1
 * first at 0x020b35a0 and D0 at 0x020b35e4.
 */
daObjBlockL_c::~daObjBlockL_c()
{
}
// @symbol _ZN13daObjBlockL_c17NotifyLinkedActorEv
/* The helper belongs to daObjBlockL_c's recovered TU and has one caller:
   daObjBlockL_c::Kill. It follows mLinkedActor and clears the per-actor state
   used by actor 0x121 or sets the equivalent state on actors 0x141..0x144. */
void daObjBlockL_c::NotifyLinkedActor()
{
    dActor_c* linkedActor = mLinkedActor;
    int actorID;
    if (linkedActor == 0)
        return;

    actorID = linkedActor->actorID;
    if (actorID <= 0x121) {
        if (actorID == 0x121)
            ((u8*)linkedActor)[0x3b0] = 0;
        return;
    }

    switch (actorID) {
    case 0x141:
    case 0x142:
    case 0x143:
    case 0x144:
        ((u8*)linkedActor)[0xd6] = 1;
    }
}

// @symbol _ZN13daObjBlockL_c16HasNonzeroAngleXEv
/* Both callers are daObjBlockL_c methods in this TU. The field at 0x8c is the
   inherited dActor_c::mAngleX; no external state participates. */
bool daObjBlockL_c::HasNonzeroAngleX()
{
    return mAngleX != 0;
}

// @symbol _ZN13daObjBlockL_c15OnHitByMegaCharER6Player
/* daObjBlockL_c::OnHitByMegaChar -- vtable slot 27, ov002 0x020b36b4.
 * include/dActor_c.h's own slot 27 supplies the signature, `virtual void
 * OnHitByMegaChar(Player &player)` -- corrected from `int` by
 * daObjPile_c::OnHitByMegaChar (include/daObjPile_c.h); re-verified here rather than
 * assumed.
 *
 * player.IncMegaKillCount() then this class's own Kill (slot 31), reached
 * through an unqualified virtual call -- the same indirect dispatch the
 * pre-migration `c->m()` vtable-slot trick reproduced. */
void daObjBlockL_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN13daObjBlockL_c8OnKickedER8dActor_c
/* daObjBlockL_c::OnKicked -- vtable slot 24, ov002 0x020b36dc.
 *
 * `void`, NOT `int`. include/dActor_c.h declared slot 24 `int` until this
 * override proved it wrong: with two locals and two early returns, mwcc
 * allocates registers differently for `int` vs `void` even though the ROM
 * sets no r0 either way -- measured with tools/mangle.py, not assumed. Every
 * other slot in this file (21/22/23/27) stayed `int` and byte-matches under
 * `int`, so this is specific to this override, not a blanket rule.
 *
 * `actorID` and `other.param1` are fBase_c's own fields. Every call is this
 * class's own Kill (slot 31) through an unqualified virtual call -- the same
 * indirect dispatch the pre-migration `self->m()` vtable-slot trick
 * reproduced. The 0x2e branch calls Kill and falls through without
 * returning, same as the pre-migration recovery.
 *
 * `int b = (actorID == 0x2e);` / `b = (actorID == 0x11);` materialize the
 * bools the same way the pre-migration recovery did -- load-bearing
 * (cpp-bool-widening-cast). */
void daObjBlockL_c::OnKicked(dActor_c &other)
{
    int p1;
    int b = (actorID == 0x2e);
    if (b) Kill();
    p1 = other.param1;
    if (p1 == 3) return;
    b = (actorID == 0x11);
    if (b) {
        if (p1 != 2) return;
        Kill();
    } else {
        Kill();
    }
}

// @symbol _ZN13daObjBlockL_c11OnAttacked2ER8dActor_c
/* daObjBlockL_c::OnAttacked2 -- vtable slot 23, ov002 0x020b3788.
 * include/dActor_c.h's own slot 23 supplies the signature, `virtual int
 * OnAttacked2(dActor_c &other)`.
 *
 * `actorID` and `other.param1` are fBase_c's own fields. Both branches call
 * this class's own Kill (slot 31) through an unqualified virtual call -- the
 * same indirect dispatch the pre-migration `(*(Vt**)c)->fn(c)` read
 * reproduced by hand.
 *
 * `int b = (actorID == 0x11);` materializes the bool the same way the
 * pre-migration recovery did -- load-bearing (cpp-bool-widening-cast). */
int daObjBlockL_c::OnAttacked2(dActor_c &other)
{
    int b = (actorID == 0x11);
    if (b) {
        if (other.param1 != 2) return;
        Kill();
        return;
    }
    Kill();
}

// @symbol _ZN13daObjBlockL_c11OnAttacked1ER8dActor_c
/* daObjBlockL_c::OnAttacked1 -- vtable slot 22, ov002 0x020b37ec. Was a plain
 * C recovery (func_ov002_020b37ec.c) with a single `void *this` parameter --
 * `other` was never read, so the pre-migration stub dropped it from the
 * declared prototype without changing the compiled bytes. The real override
 * keeps `other` in the signature (include/dActor_c.h slot 22) but still never
 * touches it.
 *
 * `actorID` is fBase_c's own field. The single call is this class's own Kill
 * (slot 31), reached through an unqualified virtual call -- the same
 * indirect dispatch the pre-migration `*(int*)((*(int*)c)+0x7c)` vtable read
 * reproduced by hand.
 *
 * `int b = (actorID == 0x11);` materializes the bool the same way the
 * pre-migration recovery did -- load-bearing (cpp-bool-widening-cast). */
int daObjBlockL_c::OnAttacked1(dActor_c &other)
{
    int b = (actorID == 0x11);
    if (b) return;
    Kill();
}

// @symbol _ZN13daObjBlockL_c15OnGroundPoundedER8dActor_c
/* daObjBlockL_c::OnGroundPounded -- vtable slot 21, ov002 0x020b382c.
 *
 * Attributed by the vtable, not the pre-migration `recovered name:` comment
 * (BigBrickBlock_OnGroundPounded, one class too far down -- see
 * actor-class-names-off-by-one). include/dActor_c.h's own slot 21 supplies
 * the signature, `virtual void OnGroundPounded(dActor_c &other)` -- corrected
 * from `int` by daObjPile_c::OnGroundPounded (include/daObjPile_c.h); re-verified here
 * rather than assumed.
 *
 * `other.param1 == 3` and `actorID == 0x11` are fBase_c's own fields, already
 * named in include/fBase_c.h. Both branches call this class's own Kill
 * (slot 31, include/daObjBlockL_c.h) through an unqualified virtual call --
 * the same indirect dispatch the pre-migration `Obj::target()` vtable-slot
 * trick reproduced. Neither branch sets r0, so the bare `return;` in this
 * `void`-returning override compiles to the same bytes as the ROM's stub,
 * which never touches r0 either.
 *
 * `int b = (actorID == 0x11);` materializes the bool into a temporary before
 * branching on it, same as the pre-migration recovery -- the temporary is
 * load-bearing (see cpp-bool-widening-cast): writing the comparison directly
 * as `if (actorID == 0x11)` compiles 3 instructions shorter than the ROM. */
void daObjBlockL_c::OnGroundPounded(dActor_c &other)
{
    if (other.param1 == 3) return;
    int b = (actorID == 0x11);
    if (b) {
        if (other.param1 != 2) return;
        Kill();
    } else {
        Kill();
    }
}

// @symbol _ZN13daObjBlockL_c4KillEv
/* daObjBlockL_c::Kill() at ov002 0x020b38a0, 0x210 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV13daObjBlockL_c (ov002 0x02108adc) carries
 * 0x020b38a0 at slot 31 -- vtable + 0x7c -- while _ZTV10dBgActor_c carries
 * _ZN10dBgActor_c4KillEv at the same slot and both tables carry dActor_c's 0x020100dc
 * at slot 30. So this is this class's own override of the one virtual dBgActor_c
 * adds. The file's old `// recovered name: BigBrickBlock_Kill` comment happened
 * to land on the right class; _ZTI13daObjBlockL_c, the RTTI record it was
 * derived from, is the same class under its Japanese name.
 *
 * ONE CLASS, FIVE ACTOR IDS, which is why this function is a switchboard rather
 * than five overrides: BrickBlock, daObjBlockL_c, BrickBlockSwitchActivated,
 * BlackBrickBlock and FortressTowerWall all allocate 816 bytes, run
 * dBgActor_c::dBgActor_c and store _ZTV13daObjBlockL_c. See include/daObjBlockL_c.h.
 *
 *   0x11  the block that holds a star. If mStarID is set it releases the star:
 *         through dActor_c::UntrackAndSpawnStar when param1's second byte is 0xff
 *         (the star was tracked, so the marker slot has to be freed too), and
 *         through dActor_c::Spawn of actor 0xb2 with the id tagged 0x40 otherwise.
 *         Its poof rides 0x96000 -- 150 20.12 units -- up instead of 0x64000,
 *         and uses particle 0xf.
 *   0x0f  and 0x11 when the star path did not fire: three coins, but only when
 *         mLinkedActor is null. NotifyLinkedActor handles that actor just below.
 *   0x2e  particle 0x48 rather than 0xa.
 *   0x13  the switch-activated one: it sets mBroken and returns WITHOUT
 *         destroying itself, because Behavior re-enables it from the event bit.
 *         Every other id falls through to MarkForDestruction.
 *
 * THE COMPARISON INTERMEDIATES ARE LOAD-BEARING, all five of them. `if (actorID
 * == 0x11)` folds the test into the branch -- cmp/beq, two instructions. The ROM
 * materialises the comparison into a register first and then tests THAT:
 * cmp/moveq #1/movne #0/cmp #0/b, five. Writing the int is what asks for the
 * second shape. Argued in full in src/actors/daObjFallBlock_c.cpp; here the
 * first one is doubly load-bearing, because the else-branch reuses the very
 * register it was materialised into for `isF || is11`.
 *
 * The last Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six.
 *
 * Particle::System::NewSimple and dActor_c::SpawnCoins stay spelled as their
 * mangled names -- both take Fix12<int> BY VALUE and declaring the true types
 * changes how the caller passes them. include/dActor_c.h says the same thing at the
 * bottom of the class, src/actors/dBgActor_c.cpp argues it in full, and
 * notes/mwccarm-codegen.md 6az is the rule. */
void daObjBlockL_c::Kill()
{
    Vector3 starPos;
    Vector3 pos;
    Vector3 coinPos;
    Vector3 dustPos;

    int is11 = (actorID == 0x11);
    if (is11 && mStarID != 0) {
        starPos.x = mPosX;
        starPos.y = mPosY;
        starPos.z = mPosZ;
        starPos.y += 0x96000;

        if (((param1 >> 8) & 0xff) == 0xff) {
            UntrackAndSpawnStar(mTrackStarID, mStarID, starPos, 4);
        } else {
            Spawn(0xb2, mStarID | 0x40, starPos, 0, mAreaId, -1);
        }
    } else {
        int isF = (actorID == 0xf);
        if (isF || is11) {
            if (mLinkedActor == 0) {
                coinPos.x = mPosX;
                coinPos.y = mPosY;
                coinPos.z = mPosZ;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, coinPos, 3, 0xf000, 0);
            }
        }
    }

    {
        Fix12i z = mPosZ;
        Fix12i y = mPosY + 0x64000;
        Fix12i x = mPosX;
        pos.x = x;
        pos.z = z;
        pos.y = y;
    }

    u32 kind = 0xa;
    int f11 = (actorID == 0x11);
    if (f11) {
        pos.y = mPosY + 0x96000;
        kind = 0xf;
    }
    int f2e = (actorID == 0x2e);
    if (f2e) {
        kind = 0x48;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kind, pos.x, pos.y, pos.z);

    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);

    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);

    NotifyLinkedActor();

    int f13 = (actorID == 0x13);
    if (f13) {
        mBroken = 1;
        return;
    }

    if (HasNonzeroAngleX()) {
        SpawnSoundObj(1);
    }
    MarkForDestruction();
}

// @symbol _ZN13daObjBlockL_c16CleanupResourcesEv
int daObjBlockL_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov002_02108ab0[mVariant].file->Release();
    data_ov002_02108ab4[mVariant].file->Release();
    return 1;
}

// @symbol _ZN13daObjBlockL_c6RenderEv
/* Event has no shared namespace declaration yet. */
int daObjBlockL_c::Render()
{
  int b = (actorID == 0x13);
  if (b != 0) {
    if (!(_ZN5Event6GetBitEj(mEventID) != 0 && mBroken == 0)) {
      return 1;
    } else {
      if (mSwitch != 0) {
        unsigned short d = (unsigned short)(mSwitch->mTimeLimit - mSwitch->mTimer);
        if (d < 0x2d) {
          if ((d & 1) != 0) return 1;
        }
      }
    }
  }
  int b2 = (actorID == 0x10);
  if (b2 != 0) {
    Vector3 v = data_ov002_021089e0;
    mModel.Render(&v);
  } else {
    mModel.Render(0);
  }
  return 1;
}

// @symbol _ZN13daObjBlockL_c8BehaviorEv
/* Event has no shared namespace declaration yet. The two unnamed collision
   setters remain address-named, and IsClsnInRangeOnScreen's by-value Fix12
   signature is a documented compiler wall, so those ABI calls stay explicit. */
int daObjBlockL_c::Behavior()
{
    int is13 = (int)(actorID == 0x13);
    if (is13 != 0) {
        if (mPrevEventBit != _ZN5Event6GetBitEj(mEventID))
            mBroken = 0;

        if (mSwitch == 0) {
            unsigned int id = 0xb;
            do {
                mSwitch = (daObjSwitch_c *)FindWithActorID(id, mSwitch);
            } while (mSwitch == 0 || mEventID != mSwitch->mEventBit);
        }

        if (_ZN5Event6GetBitEj(mEventID) == 0 || mBroken != 0) {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        } else {
            func_020393a4(&mMeshCollider, 0x15e000);
            func_02039394(&mMeshCollider, 0x64000);
            _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x150000, 0);
        }

        mPrevEventBit = _ZN5Event6GetBitEj(mEventID);
    } else {
        int v1 = 0x15e000;
        int v5 = 0x64000;
        int t;
        if ((t = (int)(actorID == 0x10)) != 0 ||
            (t = (int)(actorID == 0x11)) != 0 ||
            (t = (int)(actorID == 0x2e)) != 0) {
            v1 = 0x1c2000;
            v5 = 0x96000;
        }
        func_020393a4(&mMeshCollider, v1);
        func_02039394(&mMeshCollider, v5);
        _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x600000, 0);
    }
    return 1;
}

// @symbol _ZN13daObjBlockL_c13InitResourcesEv
/* daObjBlockL_c::InitResources -- one class, six actor IDs. mVariant selects the
 * row of the three 0xc-stride resource tables (model, collision, CLPS) that this
 * block loads; the collision scale then varies again by ID -- 0x1800 for 0x10,
 * 0x1000 for the other variant-0/1 blocks, 0x199 for variant 2.
 *
 * Actor 0x11 is the star block: the low byte of param1 is the star index (0xff
 * meaning none) and the second byte being 0xff registers a star marker.
 * Actor 0x13 takes the event bit from the low nibble of param1 instead.
 *
 * The two tail cases return 0 -- refuse to spawn: a 0x11 block whose star is
 * already taken when the player is entering level 1, and a 0x2e block above
 * y 0xdac000 in level 7 once star 1 is collected.
 *
 * The redundant compares and the reloaded actorID are the ROM's, not
 * tidiness lost in translation: folding them changes the compare shape.
 * dBgW_KcMbg::SetFile takes Fix12<int> by value (wall 6az) so it stays an
 * extern-C mangled free function.
 */
int daObjBlockL_c::InitResources()
{
    u16 id = actorID;
    u8 idx;

    switch (id) {
    case 0xf:
    case 0x10:
    case 0x13: mVariant = 0; break;
    case 0x11: mVariant = 1; break;
    case 0x12: break;
    case 0x2e: mVariant = 2; break;
    }

    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov002_02108ab0[mVariant].file), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    {
    int is11 = actorID;
    is11 = is11 == 0x11;
    if (is11 != 0) {
        int byte1;
        mStarID = param1;
        byte1 = (param1 >> 8) & 0xff;
        if (mStarID == 0xff)
            mStarID = 0;
        if (mStarID != 0 && byte1 == 0xff)
            mTrackStarID = TrackStar(mStarID, 2);
    }
    }

    idx = mVariant;
    if (idx <= 1) {
        int is10 = actorID;
        is10 = is10 == 0x10;
        if (is10 != 0) {
            KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov002_02108ab4[idx].file);
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider, kcl, &mClsnMat, 0x1800, mAngleY,
                data_ov002_02108ab8[idx].block);
        } else {
            KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov002_02108ab4[idx].file);
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
                data_ov002_02108ab8[idx].block);
        }
    } else {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov002_02108ab4[idx].file);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
            data_ov002_02108ab8[idx].block);
    }

    {
    int is13 = actorID;
    is13 = is13 == 0x13;
    if (is13 != 0)
        mEventID = param1 & 0xf;
    }
    mPrevEventBit = _ZN5Event6GetBitEj(mEventID);

    {
    Player *pl = ClosestPlayer();
    if (pl != 0 && pl->IsEnteringLevel() != 0) {
        int is11b = actorID;
        is11b = is11b == 0x11;
        if (is11b != false && HasNonzeroAngleX()) {
            if (data_0209f2f8 == 1)
                return 0;
        }
    }
    }

    {
    int is2e = actorID;
    is2e = is2e == 0x2e;
    if (is2e != false && data_0209f2f8 == 7) {
        if (data_0209f220 != 1) {
            if (IsStarCollectedInCurLevel(1) != 0)
                goto ret1;
        }
        if (mPosY >= 0xdac000)
            return 0;
    }
    }

ret1:
    return 1;
}
