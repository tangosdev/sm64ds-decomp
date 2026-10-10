//cpp
/* daDkk_c: DONKAKU / Grindel, the sliding crusher leaf of daDsnBase_c
 * (sibling daDsn_c / Thwomp in ov091). Overlay actor 162: symbols/overlay_actors.md
 * GRINDEL, ROM debug table DONKAKU. daDgr_c in this overlay is DONGURU, not
 * this class.
 *
 * mwccarm emits ordinary functions in reverse source order, so the
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group. The destructor pair is written
 * by nobody: include/daDkk_c.h defines ~daDkk_c() in the class body, and that
 * alone makes mwccarm emit D1 (0x021118c8) then D0 (0x02111928) at the bottom
 * of the section list, which is the cartridge's own order. See the header for
 * why the in-class form is load-bearing.
 *
 * Leftover:
 * - func_ov025_021119f4: `unk_39e = unk_39e - 1` and `(u8)(unk_39e - 1)`
 *   size-DIFF 0x90->0x84. `unk_39e--`, `--unk_39e` and `unk_39e -= 1` match.
 * - func_ov025_02111a84: `unk_39f = unk_39f + 1` size-DIFF 0xe0->0xd4.
 *   `unk_39f++`, `++unk_39f` and `unk_39f += 1` match. Two Vector3 locals
 *   instead of `Vector3 v[2]` stay 0xe0 and DIFF 8 words.
 * - func_ov025_02111a84: Earthquake as Fix12<int> size-DIFF 0xe0->0xe8.
 *   Particle::System::NewSimple as Fix12<int> size-DIFF 0xe0->0x104.
 *   `Fix12<int>{...}` does not compile ("( expected"). Sound::Play(3, id,
 *   pos) size-DIFF 0xe0->0xe4; the ROM calls func_0201267c.
 * - Behavior: IsClsnInRange as Fix12<int> size-DIFF 0xc0->0xdc.
 *   `Fix12<int>{0}` does not compile. The scalar (void *, Fix12i, Fix12i)
 *   call with (0, 0) matches.
 * - InitResources: writing va then vb size-DIFF 0xd0->0xcc. One GetClsnPos
 *   size-DIFF 0xd0->0xc4. The first copy is unread.
 * - func_ov091_02132ff4, func_ov091_02132e98 and func_ov091_02132e64 stay
 *   C calls into src/actors/daDsnBase_c.cpp. The other four func_ov091_*
 *   this file calls are already daDsnBase_c methods.
 * - data_ov025_02113814 is the file-table handle. g_profile_DONKAKU stays
 *   overlay data. Leaf operator new(unsigned long): fBase_c declares none.
 */

/* INCLUDE ORDER IS LOAD-BEARING, the same way it is in the base class's own TU:
 * daDkk_c.h reaches daDsnBase_c.h -> dBgActor_c.h, which includes common.h
 * BEFORE Model.h, fixing Matrix4x3 to common.h's flat `s32 m[12]' spelling --
 * the one every shard here compiled against. Do not hoist math/Matrix.h, and do
 * not move dBgCh_Lin.h above this line. */
#include "daDkk_c.h"
#include "common.h"
#include "decl_common.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. data_ov025_02113814 stays
 * the file table decl_common.h already declares. */
struct DkkModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    DkkModelFilePtr(unsigned int fileId);
    ~DkkModelFilePtr();
};
struct DkkClsnFileHandler : SharedFilePtr {
    unsigned int words[2];
    DkkClsnFileHandler(unsigned int fileId);
    ~DkkClsnFileHandler();
};
typedef char DkkModelFilePtr_size_must_be_8[sizeof(DkkModelFilePtr) == 8 ? 1 : -1];
typedef char DkkClsnFileHandler_size_must_be_8[sizeof(DkkClsnFileHandler) == 8 ? 1 : -1];

bool ApproachLinear(short &value, short target, short step);

/* decl_common.h declares func_ov091_02132e64/02132e98/02132ff4 (char *) and
 * data_ov025_02113814. The three func_ov025_* bodies below are methods; they
 * are not declared there.
 *
 * func_0201267c is void. Its enrolled definition is
 * `void func_0201267c(unsigned int id, const Vector3 *v)`, and neither call
 * here reads a result. `(int, void *)` is the shorthand that links, because
 * the symbol is extern "C". */
extern "C" {
extern void func_0201267c(int a, void *b);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *p, Fix12i a, Fix12i b);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *thiz, const Vector3 &v, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int x, int y, int z);
}

// @symbol daDkk_c_classInit
/* Reconstructed source-style name: SM64DS proves daDkk_c through RTTI,
 * allocation size, vtable identity, and the DONKAKU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Grindel_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02111cf8 loads 928 == 0x3a0 into the header's inline operator new;
 * dBgActor_c::C2, the mid-construction daDsnBase_c vptr, TextureSequence@0x324,
 * dExtShadowModel_c@0x338, and this class's vptr all come from the implicit ctor
 * the `new` inlines. The null check is the one `new` itself emits. */
extern "C" daDkk_c *daDkk_c_classInit()
{
    return new daDkk_c();
}

// @symbol _ZN7daDkk_c13InitResourcesEv
/* Vtable slot 0, override of a slot daDsnBase_c leaves pure, and this class's
 * ABI key function -- the first declared virtual that is neither inline nor
 * pure -- so defining it here is what makes this TU emit _ZTV7daDkk_c.
 *
 * Stores the file table pointer at mFileTable, calls daDsnBase_c::Init,
 * then either forces mState to a fixed "already airborne" value or runs a
 * downward raycast from the actor's own position to set mProbeHeight from the
 * collision point it finds. */
int daDkk_c::InitResources()
{
    mFileTable = (s32)data_ov025_02113814;
    int r = Init();
    if (param1 & 1) {
        mState = 6;
    } else {
        mState = 0;
        dBgCh_Lin ray;
        Vector3 va;
        Vector3 vb;
        /* Store order is the ROM's. va then vb shrinks this function 0xd0->0xcc. */
        int x = mPosX;
        vb.x = x;
        int y = mPosY;
        vb.y = y;
        int z = mPosZ;
        va.x = x;
        vb.z = z;
        va.y = y;
        va.z = z;
        vb.y = y + 0x7d0000;
        ray.SetObjAndLine(va, vb, this);
        if (ray.DetectClsn()) {
            /* First copy is unread. One GetClsnPos shrinks this function 0xd0->0xc4. */
            Vector3 p1 = ray.GetClsnPos();
            Vector3 p2 = ray.GetClsnPos();
            mProbeHeight = p2.y - 0x190000;
        }
    }
    return r;
}

// @symbol _ZN7daDkk_c8BehaviorEv
/* Vtable slot 6, the other slot daDsnBase_c leaves pure.
 *
 * Switches on mState to one of eight per-state step functions -- five shared
 * with the ov091 siblings, three private to this overlay -- then runs the
 * post-step housekeeping every daDsnBase_c leaf needs. */
int daDkk_c::Behavior()
{
    switch (mState) {
    case 0: func_ov091_02133020(); break;
    case 1: func_ov091_02132ff4((char *)this); break;
    case 2: func_ov091_02132f04(); break;
    case 3: func_ov091_02132e98((char *)this); break;
    case 4: func_ov091_02132e64((char *)this); break;
    case 5: func_ov025_02111a84(); break;
    case 6: func_ov025_021119f4(); break;
    case 7: func_ov025_021119a4(); break;
    }
    UpdateModelPosAndRotY();
    func_ov091_02133098();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0 ||
        func_ov091_02132dc0() != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN7daDkk_c19func_ov025_02111a84Ev
/* mState 5, the fall. Integrates the drop, and on reaching the stored ground
 * height snaps to it, shakes the camera, spawns the impact particle and hands
 * over to state 6. */
void daDkk_c::func_ov025_02111a84()
{
    /* Two Vector3 locals stay 0xe0 and DIFF 8 words. */
    Vector3 v[2];
    UpdatePos(0);
    if (mVertSpeed >= 0)
        mVertAccel = -0x4000;
    else
        mVertAccel = -0x8000;
    if (mPosY > unk_394)
        return;
    mPosY = unk_394;
    v[1].x = mPosX;
    v[1].y = mPosY;
    v[1].z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, v[1], 0x7d0000);
    unk_39e = 0x3c;
    unk_39f++;
    mState = 6;
    v[0].x = mPosX;
    v[0].y = mPosY;
    v[0].z = mPosZ;
    v[0].y = v[0].y + 0x3c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, v[0].x, v[0].y, v[0].z);
    func_0201267c(0xc7, &mCamSpacePosX);
}

// @symbol _ZN7daDkk_c19func_ov025_021119f4Ev
/* mState 6, the pause after landing. Counts 0x39e down; at zero it either
 * starts the next fall (state 5) or, on the fourth pass, turns to face the
 * opposite way (state 7). */
void daDkk_c::func_ov025_021119f4()
{
    unk_39e--;
    if (unk_39e != 0) return;
    if (unk_39f != 4) {
        mState = 5;
        mVertSpeed = 0x3c000;
        func_0201267c(0xf4, &mCamSpacePosX);
        return;
    }
    mState = 7;
    unk_39c = (s16)(mAngleY + 0x8000);
}

// @symbol _ZN7daDkk_c19func_ov025_021119a4Ev
/* mState 7, the turn. Steps mAngleY toward the target angle in unk_39c; once
 * ApproachLinear reports it has arrived, copies it into mPrevAngleY, clears the
 * pass counter and goes back to state 6. */
int daDkk_c::func_ov025_021119a4()
{
    int arrived = ApproachLinear(mAngleY, unk_39c, 0x400);
    if (arrived == 0) return arrived;
    mPrevAngleY = mAngleY;
    mState = 6;
    unk_39f = 0;
    unk_39e = 0x28;
    return 0x28;
}

// @symbol _ZN7daDkk_c16OnAimedAtWithEggEv
/* Vtable slot 29, override of dActor_c::OnAimedAtWithEgg. `mov r0,#0xce000; bx
 * lr'. Slot 29's return is added to pos.y (a height), same as daOts. 0xce000
 * is a Fix12i of 206.0 -- much taller than dActor_c's own default of 20.0
 * (0x14000). */
int daDkk_c::OnAimedAtWithEgg()
{
    return 0xce000;
}

/* D1 (0x021118c8) and D0 (0x02111928) are deliberately not written here.
 * include/daDkk_c.h defines ~daDkk_c() in the class body, which is what makes
 * mwccarm emit the pair in the cartridge's D1-then-D0 order with no D2. An
 * out-of-line definition emits D2, D0, D1 instead. Owning the key function
 * above drags both variants in, so no forcing scaffold is needed. */
// @symbol _ZN7daDkk_cD1Ev
// @symbol _ZN7daDkk_cD0Ev

/* Retail construction order: model 0x2dc, then collision 0x2dd.
 * mwcc emits __sinit_daDkk_c.cpp. */
DkkModelFilePtr data_ov025_02113a88(0x2dc);
DkkClsnFileHandler data_ov025_02113a90(0x2dd);
