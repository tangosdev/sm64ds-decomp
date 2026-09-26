//cpp
/**
 * Lethal Lava Land's rolling log (daObjFlMaruta_c) and the volcano
 * cannon's flame (daObj_volcanoCannon_c), in the one translation unit
 * the linker says they came from.
 *
 * One file because the runs overlap: daObjFlMaruta_c's vtable claims
 * two slots whose code sits in the middle of daObj_volcanoCannon_c's
 * address run (slot 32 -> 0x021126ac, slot 34 -> 0x02112710). The
 * merged run is 0x02112498..0x02112950.
 *
 * Emission order is ROM order: functions stay ROM-ascending under
 * `#pragma defer_codegen off`. Do not reorder. Both destructors are
 * single out-of-line definitions (volcanoCannon's header declares
 * only); the inline vague copies would land past
 * func_ov022_02112654.
 *
 * Leftover: func_ov080_* are cross-overlay helpers; data_ov022_*
 *   tables, data_0209e650 and the State table at data_ov022_02114690
 *   keep linker names.
 * Leftover: dBgW_KcMbg::SetFile keeps its mangled spelling (by-value
 *   Fix12<int> parameters, wall 6az).
 * Leftover: c + 0xf8 is the flame's tracked-actor ID, unnamed in any
 *   header.
 * Leftover: both factories stay hand-spelled (`new` would call the
 *   unavailable global `_Znwm`); the vptr stores keep the [2] form
 *   objisolate requires.
 */

#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"

#include "daObjFlMaruta_c.h"
#include "daObj_volcanoCannon_c.h"
#include "Player.h"

struct Vector3_16f;

extern "C" {

/* daObj_volcanoCannon_c's collaborators. The dCcAc_c::Init signature is
   deliberately the local spelling: the ROM name carries by-value class
   parameters (Fix12<int>), which mwccarm passes differently at the call site,
   so declaring the true types breaks the byte match. */
unsigned short DecIfAbove0_Short(unsigned short *p);
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, Fix12i c, Fix12i d, Fix12i e, const Vector3_16f *f);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *a, int b, int c, unsigned int d, unsigned int e);

/* daObjFlMaruta_c's collaborators. */
void func_020393a4(dBgW_KcMbg *p, int v);
extern int data_ov022_02112c98[];

/* Shared by the two flame helpers below. */
int RandomIntInternal(int *seed);
extern int data_0209e650;
void func_ov022_02112654(char *c);

}

/* One vtable declaration per class this TU installs, at namespace scope above
 * the factories that store them (never inside extern "C" -- a namespace-scope
 * variable is not mangled under the Itanium ABI either way). */
extern int _ZTV15daObjFlMaruta_c[];
extern int _ZTV13daObjMaruta_c[];

#pragma defer_codegen off

// @symbol _ZN15daObjFlMaruta_cD1Ev
// @symbol _ZN15daObjFlMaruta_cD0Ev
/* One out-of-line definition; D1 and D0 in cartridge order. THREE vptr
 * stores: its own, daObjMaruta_c's inlined, then dBgActor_c's, then
 * dBgActor_c's dBgW_KcMbg and Model, then dActor_c. */
daObjFlMaruta_c::~daObjFlMaruta_c()
{
}

// @symbol _ZN15daObjFlMaruta_c16CleanupResourcesEv
int daObjFlMaruta_c::CleanupResources()
{
    return func_ov080_021270dc(((void *)this), data_ov022_02112c9c);
}

// @symbol _ZN15daObjFlMaruta_c8BehaviorEv
int daObjFlMaruta_c::Behavior()
{
    func_020393a4(&mMeshCollider, 0x500000);
    return func_ov080_0212714c(((char *)this), data_ov022_02112c98) & 0xff;
}

// @symbol _ZN15daObjFlMaruta_c13InitResourcesEv
int daObjFlMaruta_c::InitResources()
{
    return func_ov080_021274ac(((void *)this), data_ov022_02112c9c);
}

// @symbol daObjFlMaruta_c_classInit
extern "C" int *daObjFlMaruta_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(836);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjMaruta_c;
        p[0] = (int)&_ZTV15daObjFlMaruta_c[2];
    }
    return p;
}

// @symbol _ZN21daObj_volcanoCannon_cD1Ev
// @symbol _ZN21daObj_volcanoCannon_cD0Ev
/* One out-of-line definition; D1 and D0 in cartridge order. The body is
 * empty: vptr store, dCcAc_c member teardown and base chain from the
 * class layout; D0's deallocation is the inline operator delete. */
daObj_volcanoCannon_c::~daObj_volcanoCannon_c()
{
}

// @symbol func_ov022_02112654
/* Burns Mario (actor type 0xbf) if the tracked actor is Mario, then
 * retires itself. */
extern "C" void func_ov022_02112654(char *c)
{
    if (*(unsigned int *)(c + 0xf8) == 0) return;
    void *a = dActor_c::FindWithID(*(unsigned int *)(c + 0xf8));
    if (a == 0) return;
    unsigned int b = ((dActor_c *)a)->actorID == 0xbf;
    if (b == 0) return;
    ((Player *)a)->Burn();
    ((fBase_c *)c)->MarkForDestruction();
}

// @symbol func_ov022_021126ac
/* daObjFlMaruta_c vtable slot 32. Historical alias:
 * daObjFlMaruta_c_AfterClsn. */
extern "C" int func_ov022_021126ac(char *c)
{
    int a = *(int *)(c + 0x60);
    int b = *(int *)(c + 0x118);
    if (a < b) {
        ((fBase_c *)c)->MarkForDestruction();
        return 1;
    }
    unsigned short h = *(unsigned short *)(c + 0x100 + 0x10);
    if (h == 0) {
        ((fBase_c *)c)->MarkForDestruction();
        return 1;
    }
    func_ov022_02112654(c);
    return 1;
}

// @symbol func_ov022_02112710
/* daObjFlMaruta_c vtable slot 34. Rolls the log back to a randomised
 * start: a 0x3c timer, a random facing, and randomised speed and range
 * in 12-bit fixed point. */
extern "C" int func_ov022_02112710(char *c)
{
    unsigned int r = (unsigned int)RandomIntInternal(&data_0209e650) >> 8;
    int *p98;
    int *pa8;
    *(short *)(c + 0x110) = 0x3c;
    *(short *)(c + 0x94) = (short)((r & 0xf) << 0xc);
    *(int *)(c + 0x98) = (r & 0x1f) + 0x14;
    p98 = (int *)((int)c + 0x98);
    *p98 = *p98 << 0xc;
    *(int *)(c + 0x9c) = -0x4000;
    *(int *)(c + 0xa8) = (r & 0x3f) + 0x28;
    pa8 = (int *)((int)c + 0xa8);
    *pa8 = *pa8 << 0xc;
    return 1;
}

// @symbol _ZN21daObj_volcanoCannon_c11ChangeStateEPNS_5StateE
/* The state record contains two pointer-to-member callbacks; the
 * transition stores the record and runs its init. */
int daObj_volcanoCannon_c::ChangeState(State *state)
{
    mState = state;
    if (mState->init == 0)
        return 1;
    return (this->*mState->init)();
}

// @symbol _ZN21daObj_volcanoCannon_c16CleanupResourcesEv
/* Vtable slot 3. Hands its slot back to the spawner that made it, by
 * decrementing the live-flame count the spawner keeps at +0x324.
 * mSpawner is null for a flame that was never registered. */
int daObj_volcanoCannon_c::CleanupResources()
{
    if (mSpawner)
        *(u16 *)((char *)mSpawner + 0x324) -= 1;
    return 1;
}

// @symbol _ZN21daObj_volcanoCannon_c8BehaviorEv
int daObj_volcanoCannon_c::Behavior()
{
    DecIfAbove0_Short(&mKillTimer);
    if (mState->behavior != 0)
        (this->*mState->behavior)();
    UpdatePos(&mCylinderClsn);
    mCylinderClsn.Clear();
    mCylinderClsn.Update();
    {
        int b = (int)((mFlags & 8) != 0);
        if (b == 0) {
            mParticleID = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticleID, 0x129, mPosX, mPosY, mPosZ, 0);
        }
    }
    return 1;
}

// @symbol _ZN21daObj_volcanoCannon_c13InitResourcesEv
int daObj_volcanoCannon_c::InitResources()
{
    mTerminalVelocity = -0xc8000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mCylinderClsn, this, 0x1e000, 0x1e000, 0x200002, 0);
    ChangeState((State *)&data_ov022_02114690);
    return 1;
}

// @symbol daObj_volcanoCannon_c_classInit
extern "C" int *daObj_volcanoCannon_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x11c);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV21daObj_volcanoCannon_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
