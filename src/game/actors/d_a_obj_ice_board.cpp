//cpp
/* Production translation unit for ov018/daObjIceBoard_c, hand-curated.
 * 10 function(s), .text 0x021127bc..0x02112a74.
 *
 * Cool Cool Mountain's shatterable ice sheet (profile ICE_BOARD, actor 295):
 * a mesh collider you can stand on until a ground pound or a mega-Mario hit
 * breaks it, at which point it plays one sound, throws three particle systems
 * and destroys itself.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov018 0x02113afc  "15daObjIceBoard_c"
 *   _ZTI  ov018 0x02113af0  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov018 0x02113b34  the ADDRESS POINT: V-8 is a zero offset-to-top,
 *                           V-4 is &_ZTI, V+0 is slot 0 (InitResources,
 *                           0x021129c0). 32 slots; the emitted object begins
 *                           at 0x02113b2c.
 *   size  0x320             daObjIceBoard_c_classInit's own literal, which is
 *                           dBgActor_c's own size too: this class adds no
 *                           fields.
 * The coined `IceSheet` alias that used to sit on this vtable is gone; the
 * ROM's own type string is where the class name now comes from.
 *
 * Nine slots point inside ov018 -- 0, 3, 6, 9, 16, 17, 21, 27 and 31 -- and
 * every other slot still holds dBgActor_c's arm9 word, so nothing else is
 * overridden.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in
 * include/daObjIceBoard_c.h, declared last, and so emits first, which is
 * where the cartridge has it.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x021127bc  src/_ZN15daObjIceBoard_cD1Ev.cpp
 *   [1] 0x02112800  src/_ZN15daObjIceBoard_cD0Ev.cpp
 *   [2] 0x02112858  src/_ZN15daObjIceBoard_c15OnHitByMegaCharER6Player.cpp
 *   [3] 0x02112880  src/_ZN15daObjIceBoard_c4KillEv.cpp
 *   [4] 0x021128e0  src/_ZN15daObjIceBoard_c15OnGroundPoundedER8dActor_c.cpp
 *   [5] 0x02112924  src/_ZN15daObjIceBoard_c16CleanupResourcesEv.cpp
 *   [6] 0x02112968  src/_ZN15daObjIceBoard_c6RenderEv.cpp
 *   [7] 0x02112990  src/_ZN15daObjIceBoard_c8BehaviorEv.cpp
 *   [8] 0x021129c0  src/_ZN15daObjIceBoard_c13InitResourcesEv.cpp
 *   [9] 0x02112a44  src/daObjIceBoard_c_classInit.c
 */

#include "daObjIceBoard_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr daObjIceBoard_c_ClsnFile;
extern SharedFilePtr daObjIceBoard_c_ModelFile;
extern CLPS_Block data_ov002_0210d754;

/* Particle::System::NewSimple's true signature takes Fix12<int> by value.
 * Declaring that on this TU's Kill does not compile: mPosX/Y/Z are s32 and
 * mwccarm refuses the implicit conversion. The scalar ABI spelling is the
 * measured form (notes/mwccarm-codegen.md 6az). */
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);

/* dBgW_KcMbg::SetFile likewise takes Fix12<int> by value. A real member
 * call on this TU's InitResources is 9/10 MATCH -- InitResources DIFFs. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angY, CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjIceBoard_c_classInit
/* Every instruction the cartridge has here falls out of the one `new`.
 * 0x02112a48 loads 0x320 -- the class's own size -- into the header's leaf
 * operator new; 0x02112a58 calls dBgActor_c's C2 and the store at 0x02112a64
 * lays down this class's vptr. The null check is the one `new` itself emits.
 * The implicit constructor is what keeps the intervening dBgActor_c
 * construction inlined; declaring a constructor of our own would emit a `bl`
 * the factory does not have. */
extern "C" daObjIceBoard_c *daObjIceBoard_c_classInit()
{
    return new daObjIceBoard_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_c13InitResourcesEv
int daObjIceBoard_c::InitResources()
{
    void *modelFile = Model::LoadFile(daObjIceBoard_c_ModelFile);
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(daObjIceBoard_c_ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x199, mAngleY,
        &data_ov002_0210d754);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_c8BehaviorEv
int daObjIceBoard_c::Behavior()
{
    if (!mMeshCollider.IsEnabled())
        mMeshCollider.Enable(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_c6RenderEv
int daObjIceBoard_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_c16CleanupResourcesEv
int daObjIceBoard_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    daObjIceBoard_c_ModelFile.Release();
    daObjIceBoard_c_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_c15OnGroundPoundedER8dActor_c
/* daObjIceBoard_c::OnGroundPounded -- vtable slot 21, ov018 0x021128e0.
 *
 * param1 is the player's character id; 2 is Wario. The trailing unqualified
 * Kill() reaches this class's own slot-31 override through the vtable. */
void daObjIceBoard_c::OnGroundPounded(dActor_c &other)
{
    if (&other == 0) return;
    if (other.param1 != 2) return;
    Kill();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_c4KillEv
/* daObjIceBoard_c::Kill() at ov018 0x02112880, 0x60 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV15daObjIceBoard_c (ov018 0x02113b34) carries
 * 0x02112880 at slot 31 (0x02113bb0) and _ZTV10dBgActor_c carries
 * _ZN10dBgActor_c4KillEv at the same slot, so this is this class's own
 * override of dBgActor_c's Kill. The RTTI agrees on the class:
 * _ZTI15daObjIceBoard_c (ov018 0x02113af0) is a __si_class_type_info with ONE
 * base at subobject offset 0, ov002 0x021089ec -- dBgActor_c.
 *
 * The sheet shatters: the sound first, then three particle systems -- 0x74,
 * 0x75, 0x76 -- all at the actor's own position, and then it is gone.
 *
 * THE POSITION IS RE-READ PER CALL. This TU's Kill issues three `ldr` from
 * [this,#0x5c/#0x60/#0x64] ahead of each of the three calls, nine loads in
 * all, rather than loading the triple once into callee-saved registers.
 * Hoisting them into locals is one instruction shorter per call and does not
 * reproduce; naming the fields at each call site is what asks for the loads
 * the cartridge has.
 *
 * mCamSpacePos is read as a Vector3 through its first member, which is how
 * the rest of the tree spells that triple. */
void daObjIceBoard_c::Kill()
{
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x74, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x75, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x76, mPosX, mPosY, mPosZ);
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_c15OnHitByMegaCharER6Player
/* daObjIceBoard_c::OnHitByMegaChar -- vtable slot 27, ov018 0x02112858.
 *
 * Player::IncMegaKillCount is a real method, and the trailing unqualified
 * Kill() reaches this class's own slot-31 override. */
void daObjIceBoard_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjIceBoard_cD1Ev
// @symbol _ZN15daObjIceBoard_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjIceBoard_c.h and declared LAST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it last, with the factory's `new` instantiating the class, is
 * what emits the used D1/D0 pair in cartridge order and keeps this TU the
 * vtable's home.
 *
 * Both bodies are short because the chain is short and this class adds no
 * member of its own: this class's vptr store, then dBgActor_c's -- inlined,
 * its destructor is defined in its class body -- then dBgActor_c's Model and
 * dBgW_KcMbg, then dActor_c. D0's trailing deallocation is the inherited
 * inline operator delete, which is why nothing here names a heap.
 */
