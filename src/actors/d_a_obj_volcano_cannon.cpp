//cpp
/* ov022 / daObj_volcanoCannon_c -- the flame a volcano cannon spits
 * (registry profile OBJ_VOLCANO_CANNON, RTTI name at ov022:0x02114440).
 *
 * One translation unit for the whole ROM run 0x021125e0..0x02112950: the
 * destructor pair, the three state callbacks the class's State table names,
 * the state dispatcher, the three virtual leaves and the factory, plus the
 * .data run 0x02114434..0x02114498 that holds the class's RTTI chain, its
 * profile descriptor and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daObj_volcanoCannon_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 */
#include "decl_common.h"
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"

#include "daObj_volcanoCannon_c.h"

struct Vector3_16f;

/* Reconciled from the ten per-function copies. Every one of these is a ROM
 * symbol spelled by its final mangled name, so it needs C linkage: seen as C++
 * the compiler would mangle the already-mangled spelling a second time and emit
 * a reference nothing defines. */
extern "C" {
extern void  _ZN6Player4BurnEv(void *player);
extern void  _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern int   RandomIntInternal(int *seed);
extern int   data_0209e650;
extern void  _ZN5dCc_c5ClearEv(void *cc);
extern void  _ZN5dCc_c6UpdateEv(void *cc);
extern void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, dCc_c *cc);
extern unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        unsigned int id, unsigned int kind, Fix12i x, Fix12i y, Fix12i z,
        const Vector3_16f *v);
/* Signature deliberately copied from the legacy declaration: the ROM name
 * carries by-value class parameters (Fix12<int>), which mwccarm passes
 * differently at the call site, so declaring the true types breaks the byte
 * match. See notes/mwccarm-codegen.md 6az. */
extern void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        void *self, dActor_c *owner, int r, int h, unsigned int flags,
        unsigned int mask);

/* Defined below, and named by this class's State table at ov022:0x02114690. */
void func_ov022_02112654(char *c);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 9 -- daObj_volcanoCannon_c_classInit, 0x02112918, size 0x38    */
/* ------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObj_volcanoCannon_c through
 * its ROM RTTI string, allocation size, vtable identity, and the
 * OBJ_VOLCANO_CANNON registry profile; later EAD lineage supplies classInit.
 * Exact original spelling is not preserved. Historical alias:
 * VolcanoFire_Spawn.
 *
 * The factory stays hand-spelled rather than `new daObj_volcanoCannon_c()`:
 * a natural new-expression makes mwccarm call unresolved `_Znwm`, where the
 * ROM calls the class allocator `_ZN7fBase_cnwEj`. */
// @symbol daObj_volcanoCannon_c_classInit
extern "C" int *daObj_volcanoCannon_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x11c);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x02114474 and +8 is what reaches the
         * 0x0211447c address point the ROM stores. The addend-0 spelling was
         * right only while the class merely IMPORTED its vtable. */
        p[0] = (int)&_ZTV21daObj_volcanoCannon_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}

/* ------------------------------------------------------------------------- */
/* g_profile_OBJ_VOLCANO_CANNON, 0x02114458, size 0x1c                       */
/* ------------------------------------------------------------------------- */
/* .data objects emit in SOURCE order, so the descriptor is written directly
 * after the factory it names -- which is where the ROM's run puts it, between
 * _ZTS (0x02114440) and the vtable object (0x02114474). */
struct VolcanoCannonSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x00f3 */
    s16 drawOrder;                  /* 0x0065 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char VolcanoCannonSpawnInfo_size_must_be_0x1c[
    sizeof(VolcanoCannonSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_OBJ_VOLCANO_CANNON
extern "C" VolcanoCannonSpawnInfo g_profile_OBJ_VOLCANO_CANNON = {
    daObj_volcanoCannon_c_classInit, 0x00f3, 0x0065, 0x00000002,
    0x00010000, 0x00060000, 0x04000000, 0x00000000
};

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daObj_volcanoCannon_c::InitResources, 0x021128b8, 0x60    */
/* ------------------------------------------------------------------------- */
// @symbol _ZN21daObj_volcanoCannon_c13InitResourcesEv
int daObj_volcanoCannon_c::InitResources()
{
    mTerminalVelocity = -0xc8000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinderClsn, this, 0x1e000, 0x1e000, 0x200002, 0);
    ChangeState((State *)&data_ov022_02114690);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObj_volcanoCannon_c::Behavior, 0x02112800, size 0xb8    */
/* ------------------------------------------------------------------------- */
/* Runs down the kill timer, runs the current state's per-frame callback,
 * pushes the flame's position into its cylinder collider, and -- unless the
 * actor is flagged out at bit 3 -- keeps particle 0x129 burning at its feet. */
// @symbol _ZN21daObj_volcanoCannon_c8BehaviorEv
int daObj_volcanoCannon_c::Behavior()
{
    DecIfAbove0_Short(&mKillTimer);
    if (mState->behavior != 0)
        (this->*mState->behavior)();
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mCylinderClsn);
    _ZN5dCc_c5ClearEv(&mCylinderClsn);
    _ZN5dCc_c6UpdateEv(&mCylinderClsn);
    {
        int b = (int)((mFlags & 8) != 0);
        if (b == 0) {
            mParticleID =
                _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                    mParticleID, 0x129, mPosX, mPosY, mPosZ, 0);
        }
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObj_volcanoCannon_c::CleanupResources, 0x021127e0, 0x20 */
/* ------------------------------------------------------------------------- */
/* The only one of these leaves that owns no files: it hands its slot back to
 * the spawner that made it, by decrementing the live-flame count that spawner
 * keeps at +0x324. mSpawner is null for a flame that was never registered,
 * hence the guard. The spawner's own class is not described by any header yet,
 * so that offset stays raw. */
// @symbol _ZN21daObj_volcanoCannon_c16CleanupResourcesEv
int daObj_volcanoCannon_c::CleanupResources()
{
    if (mSpawner)
        *(u16 *)((char *)mSpawner + 0x324) -= 1;
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daObj_volcanoCannon_c::ChangeState, 0x02112790, size 0x50 */
/* ------------------------------------------------------------------------- */
/* The state record holds two pointer-to-member callbacks; a transition stores
 * the record and runs its init half. */
// @symbol _ZN21daObj_volcanoCannon_c11ChangeStateEPNS_5StateE
int daObj_volcanoCannon_c::ChangeState(State *state)
{
    mState = state;
    if (mState->init == 0)
        return 1;
    return (this->*mState->init)();
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov022_02112710, 0x02112710, size 0x80               */
/* ------------------------------------------------------------------------- */
/* A state init half: gives the flame 0x3c frames to live and throws it out of
 * the cannon on a random heading with a random speed and upward kick. */
// @symbol func_ov022_02112710
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

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov022_021126ac, 0x021126ac, size 0x64               */
/* ------------------------------------------------------------------------- */
/* The matching per-frame half: the flame dies when it falls past the height
 * mKillPosY records or when its timer runs out, and otherwise checks whether
 * it has caught a player. */
// @symbol func_ov022_021126ac
extern "C" int func_ov022_021126ac(char *c)
{
    int a = *(int *)(c + 0x60);
    int b = *(int *)(c + 0x118);
    if (a < b) {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }
    {
        unsigned short h = *(unsigned short *)(c + 0x100 + 0x10);
        if (h == 0) {
            _ZN7fBase_c18MarkForDestructionEv(c);
            return 1;
        }
    }
    func_ov022_02112654(c);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov022_02112654, 0x02112654, size 0x58               */
/* ------------------------------------------------------------------------- */
/* The collision half: whatever the cylinder collider last touched is looked up
 * by actor id, and a player (type 0xbf) is set on fire; the flame spends itself
 * doing it. */
// @symbol func_ov022_02112654
extern "C" void func_ov022_02112654(char *c)
{
    if (*(unsigned int *)(c + 0xf8) == 0) return;
    {
        void *a = _ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0xf8));
        if (a == 0) return;
        {
            unsigned int b = *(unsigned short *)((char *)a + 0xc) == 0xbf;
            if (b == 0) return;
        }
        _ZN6Player4BurnEv(a);
    }
    _ZN7fBase_c18MarkForDestructionEv(c);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN21daObj_volcanoCannon_cD1Ev  0x021125e0  size 0x30  (complete object) */
/*   _ZN21daObj_volcanoCannon_cD0Ev  0x02112610  size 0x44  (deleting)        */
/* ------------------------------------------------------------------------- */
/* Defined in include/daObj_volcanoCannon_c.h's class body, and deliberately
 * NOT repeated out of line here: out of line mwccarm emits D2, D0, D1, and the
 * cartridge holds D1, D0 with no D2 at all. The body is the vtable store, the
 * dCcAc_c member's own destructor and the base subobject teardown. D0
 * additionally returns the object to its heap through the inline operator
 * delete it inherits, which is why nothing mentions a heap. */

// @symbol _ZN21daObj_volcanoCannon_cD0Ev
// @symbol _ZN21daObj_volcanoCannon_cD1Ev
