//cpp
/* Production translation unit for ov064/daObjShell_c.
 * 8 function(s), .text 0x0211a930..0x0211adb0.
 *
 * The giant clams of Jolly Roger Bay and the sunken ship (profile OBJ_SHELL).
 * A clam idles shut; once its state timer passes 150 frames and the player is
 * inside 500.0 it puffs, swaps to the lunge animation and opens. Its damage
 * cylinder is armed only for the part of that lunge that can connect, and
 * anything it touches carrying the player's actor ID is hurt for 2.
 *
 * WHAT THE CARTRIDGE PROVES:
 *   _ZTS  ov064 0x0211c594  "12daObjShell_c"
 *   _ZTI  ov064 0x0211c588  __si_class_type_info; one direct base, dActor_c
 *                           at arm9:0x0208e390, at offset zero
 *   _ZTV  ov064 0x0211c5c8  the address point; 31 slots, the same extent as
 *                           dActor_c, with 0, 3, 6, 9, 16 and 17 overridden
 *   profile ov064 0x0211c5a4  g_profile_OBJ_SHELL, whose factory word is
 *                           daObjShell_c_classInit at 0x0211ad70
 *   size  0x174             the literal the factory hands operator new
 * The RTTI name is the ROM's own; no class name here is coined.
 *
 * WHY THIS IS ONE TRANSLATION UNIT: the run 0x0211a930..0x0211adb0 is
 * contiguous and ends this overlay's .text. Its left edge is the first
 * instruction after daObjTbox_c's own factory, where that already promoted TU
 * stops; its right edge is the end of the section. Every function inside it is
 * either a named daObjShell_c member, this class's factory, or the one
 * still-unnamed helper at 0x0211a9b4 that InitResources calls -- no foreign
 * class appears anywhere in the span. The factory run that follows
 * InitResources carries no mangled name for tu_map to attribute, but it
 * allocates this class's own 0x174, installs this class's vptr, constructs
 * this class's two members and is the address the OBJ_SHELL profile stores --
 * the same registry/factory-backed join ov064/daObjTbox_c was promoted on.
 * config/arm9/overlays/ov064/delinks.txt enrolled all eight as adjacent
 * `complete` entries with no hole, so the whole run could be claimed at once.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder. The
 * inline destructor in include/daObjShell_c.h emits the retail D1/D0 pair
 * first and emits no D2 body.
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim, dCcAc_c::Init, Particle::System::NewSimple and
 *   Player::Hurt keep their C ABI spellings: their real declarations pass
 *   Fix12<int> by value, which mwccarm lowers differently at a C++ call site.
 * - func_0201267c is still a linker name; it is the positional-sound entry
 *   point both state transitions use.
 * - func_ov064_0211a9b4 is this TU's own still-unnamed helper. Its body is
 *   structurally the twin of the already named
 *   daObjTbox_c::UpdateModelTransform two TUs earlier in this same overlay,
 *   but the cartridge records no name for it, and coining one is a rename PR
 *   rather than this one. It is typed against the class here so it at least
 *   reads as the member it is.
 * - SharedFilePtr has no recovered layout, so the BCA_File handle each SetAnim
 *   call passes is still read as the raw word at +4.
 * - the three SharedFilePtr handles at 0x0211c9bc/0x0211c9c4/0x0211c9cc are
 *   overlay data this TU consumes and does not own.
 *
 * Consolidated from eight one-function sources, one per symbol below. Their
 * retired paths are recorded per function in this TU's manifest entry; naming
 * them here would leave the comment pointing at files the fold deletes.
 * ROM address order:
 *   [0] 0x0211a930  _ZN12daObjShell_cD1Ev
 *   [1] 0x0211a968  _ZN12daObjShell_cD0Ev
 *   [2] 0x0211a9b4  func_ov064_0211a9b4
 *   [3] 0x0211a9f4  _ZN12daObjShell_c16CleanupResourcesEv
 *   [4] 0x0211aa30  _ZN12daObjShell_c6RenderEv
 *   [5] 0x0211aa58  _ZN12daObjShell_c8BehaviorEv
 *   [6] 0x0211acc4  _ZN12daObjShell_c13InitResourcesEv
 *   [7] 0x0211ad70  daObjShell_c_classInit
 */

#include "daObjShell_c.h"
#include "SharedFilePtr.h"
#include "dCc_c.h"

extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *, s32);
/* The positional-sound entry point; takes the actor's camera-space position. */
void func_0201267c(int soundId, Fix12i *camSpacePos);
int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed, u32 startFrame);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);
/* `char *` on the receiver, not `dActor_c *`: that is the spelling this ABI
 * seam already carries at most of its call sites, and a promotion is not the
 * place to change a shared declaration. */
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    char *player, int *src, unsigned damage, int knockback,
    unsigned a, unsigned b, unsigned c);

/* This overlay's three file handles: the model, the lunge animation and the
 * idle animation. InitResources loads all three, CleanupResources releases
 * them, and Behavior swaps between the two animations. */
extern SharedFilePtr data_ov064_0211c9bc;
extern SharedFilePtr data_ov064_0211c9c4;
extern SharedFilePtr data_ov064_0211c9cc;

/* This TU's own helper, still carrying its linker name -- see the deslop note
 * above. Declared here because InitResources calls it and is written first.
 * `char *` is the spelling include/decl_common.h gives it tree-wide; the
 * definition below takes its typed view of the object internally. */
void func_ov064_0211a9b4(char *clam);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjShell_c_classInit, 0x0211ad70, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjShell_c_classInit
/* Reconstructed source-style name: the cartridge proves daObjShell_c through
 * RTTI, allocation size and vtable identity, and the OBJ_SHELL profile at
 * 0x0211c5a4 takes this function's address as its factory slot; later EAD
 * lineage supplies the classInit spelling. The exact original spelling is not
 * preserved. Historical alias: Clam_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`. The
 * literal 0x174 is the class's own size, handed to the leaf operator new the
 * header declares; then dActor_c's C2, this class's vptr store, and the
 * ModelAnim (0x0d4) and dCcAc_c (0x138) member constructors, in declaration
 * order. The null check is the one `new` itself emits, and the implicit
 * default constructor is what keeps that construction inlined -- declaring a
 * constructor of our own would emit a `bl` the factory does not have.
 *
 * The legacy one-function file this replaces had to spell the allocation as a
 * direct call to the mangled fBase_c::operator new, because without a leaf
 * `operator new` in scope `new daObjShell_c` reached for the global one. The
 * header now declares that leaf, so the factory is a plain `new` and the ROM's
 * own allocator call is what the compiler emits.
 */
extern "C" daObjShell_c *daObjShell_c_classInit()
{
    return new daObjShell_c();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN12daObjShell_c13InitResourcesEv, 0x0211acc4, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjShell_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Both animations are loaded, then the model; the clam starts on the idle
 * animation at 0x0211c9bc. The cylinder is 0x64000 in both radius and height.
 */
int daObjShell_c::InitResources()
{
    Animation::LoadFile(data_ov064_0211c9cc);
    Animation::LoadFile(data_ov064_0211c9bc);
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov064_0211c9c4);
    mModelAnim.SetFile(modelFile, 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        *(BCA_File **)((u8 *)&data_ov064_0211c9bc + 4),
        0x40000000, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x64000, 0x64000, 0x200004, 0);
    func_ov064_0211a9b4((char *)this);
    mState = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12daObjShell_c8BehaviorEv, 0x0211aa58, size 0x26c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjShell_c8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The clam's two-state open/shut cycle. State 0 is shut: on animation frame
 * 0x19 it plays sound 0x27, puffs particle 0x10c above itself, arms a
 * 10-frame counter and CLEARS the collision bit; otherwise, once the state
 * timer passes 150 frames and the player is within 0x1f4000 (500.0), it
 * plays sound 0x26, puffs particle 0x10b and switches to state 1 on the
 * other animation. State 1 runs 150 frames and goes back, setting the
 * collision bit on animation frame 0xf but NOT on frame 8 -- the bit is
 * live only for the part of the lunge that can connect.
 *
 * Whatever the state, a touched actor whose ID is 0xbf (the player) is hurt
 * for 2 with 0xc000 of knockback, every frame the contact persists.
 *
 * mModelAnim carries the Animation base at +0x50, so the ROM's calls on
 * `this + 0x124` are member calls on `this + 0xd4`: the compiler applies
 * the same adjustment the hand-spelt offsets did.
 *
 * THE `(int)` CAST ON THE ACTOR-ID COMPARISON IS LOAD-BEARING, and it is C++
 * that makes it so. In C, `actorID == 0xbf` is already an `int`; compiled as
 * C++ the comparison is a `bool`, and widening THAT instead moves the sixth
 * argument of the Hurt call into a different register -- 17 words out of 155.
 * It was the only one of six candidate spellings that matched. The function's
 * four other round-trips -- the two on the collision word, the counter
 * decrement and the timer increment -- were not load-bearing and are gone;
 * one compile settles each, and the idiom is per-site.
 */
int daObjShell_c::Behavior()
{
    struct {
        int shutPuff[3];
        int lungePuff[3];
        int hurtFrom[3];
    } u;

    switch (mState) {
    case 0:
        if (mModelAnim.WillHitFrame(0x19)) {
            func_0201267c(0x27, &mCamSpacePosX);
            u.shutPuff[0] = mPosX;
            u.shutPuff[1] = mPosY;
            u.shutPuff[2] = mPosZ;
            u.shutPuff[1] = u.shutPuff[1] + 0x32000;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10c,
                u.shutPuff[0], u.shutPuff[1], u.shutPuff[2]);
            mShutTimer = 0xa;
            mStateTimer = 0;
            mdCcAc_c.flags &= ~1;
        } else {
            if (mStateTimer > 0x96 &&
                DistToCPlayer() < 0x1f4000) {
                func_0201267c(0x26, &mCamSpacePosX);
                u.lungePuff[0] = mPosX;
                u.lungePuff[1] = mPosY;
                u.lungePuff[2] = mPosZ;
                u.lungePuff[1] = u.lungePuff[1] + 0x32000;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10b,
                    u.lungePuff[0], u.lungePuff[1], u.lungePuff[2]);
                mStateTimer = 0;
                mState = 1;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim,
                    *(BCA_File **)((u8 *)&data_ov064_0211c9cc + 4),
                    0x40000000, 0x1000, 0);
            } else {
                if (mShutTimer != 0)
                    mShutTimer--;
            }
        }
        break;
    case 1:
        if (mStateTimer > 0x96) {
            mState = 0;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim,
                *(BCA_File **)((u8 *)&data_ov064_0211c9bc + 4),
                0x40000000, 0x1000, 0);
            mStateTimer = 0;
        } else {
            if (!mModelAnim.WillHitFrame(8)) {
                if (mModelAnim.WillHitFrame(0xf))
                    mdCcAc_c.flags |= 1;
            }
        }
        break;
    }

    mStateTimer++;

    if (mdCcAc_c.otherOwner != 0) {
        dActor_c *touched = dActor_c::FindWithID(mdCcAc_c.otherOwner);
        if (touched != 0) {
            int isPlayer = (int)(touched->actorID == 0xbf);
            if (isPlayer != 0) {
                u.hurtFrom[0] = mPosX;
                u.hurtFrom[1] = mPosY;
                u.hurtFrom[2] = mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((char *)touched,
                    u.hurtFrom, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    mModelAnim.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN12daObjShell_c6RenderEv, 0x0211aa30, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjShell_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daObjShell_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN12daObjShell_c16CleanupResourcesEv, 0x0211a9f4, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjShell_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Three releases, all of this overlay's own handles, and they are the same
 * three InitResources loads: the model at 0211c9c4 and the two animations at
 * 0211c9cc (the lunge) and 0211c9bc (the idle) that Behavior switches
 * between. InitResources loads them animations-first; the release order here
 * is model-first, which is the ROM's and is reproduced verbatim.
 */
int daObjShell_c::CleanupResources()
{
    data_ov064_0211c9c4.Release();
    data_ov064_0211c9cc.Release();
    data_ov064_0211c9bc.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov064_0211a9b4, 0x0211a9b4, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_0211a9b4
/* Rebuilds the model's world matrix from the actor's facing angle and its
 * position, the position scaled down by 8 the way every ModelAnim actor in
 * this overlay does it. InitResources calls it once, at spawn.
 *
 * Still carries its linker name: daObjTbox_c, promoted just before this TU in
 * the same overlay, has a structurally identical helper named
 * UpdateModelTransform, but the cartridge records no name for this one and
 * coining one belongs in a rename PR. It keeps C linkage, so the symbol the
 * delinker configured is exactly the symbol this TU defines. */
extern "C" void func_ov064_0211a9b4(char *clam)
{
    daObjShell_c *shell = (daObjShell_c *)clam;

    Matrix4x3_FromRotationY(&shell->mModelAnim.mat4x3, shell->mAngleY);
    shell->mModelAnim.mat4x3.t.x = shell->mPosX >> 3;
    shell->mModelAnim.mat4x3.t.y = shell->mPosY >> 3;
    shell->mModelAnim.mat4x3.t.z = shell->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN12daObjShell_cD0Ev, 0x0211a968, size 0x4c */
/* ROM ordinal 0 -- _ZN12daObjShell_cD1Ev, 0x0211a930, size 0x38 */
/* -------------------------------------------------------------------------- */
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjShell_c() {}` in the header is
 * the whole source of both variants: from an inline body mwccarm emits D1 and
 * then D0 -- the cartridge's own order -- and no leaf D2. Written out of line
 * here instead, the two legacy shards' bodies reproduce the same bytes but the
 * variants come out D0-before-D1, and the isolation step rejects the object.
 *
 * Every instruction in both is compiler-generated teardown implied by
 * `struct daObjShell_c : dActor_c`: this class's own vptr store, then dCcAc_c
 * at 0x138 and ModelAnim at 0x0d4 in reverse construction order, then the
 * dActor_c base. D0 additionally returns the object to the actor heap through
 * the inline operator delete.
 */
