/* class dScMgJump2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgJump2_c : dScMgD3DBase_c, single edge, offset 0 (build/rtti.json).
 * Its own vtable is ov006:0x0213ccfc.
 *
 * SIZE 0x5a78 -- AND ITS FACTORY USED TO CARRY ANOTHER CLASS'S NAME ENTIRELY.
 * The allocation lives in `dScMgJump2_c_classInit` (ov006:0x020efaf0, renamed from
 * the mangled `_ZN8PathLift17BaseInitResourcesEv`, which was neither a PathLift
 * method nor a BaseInitResources): its body is `operator new(0x5a78)`,
 * dScMgBase_c's constructor, dScMgD3DBase_c's vtable, mSysTracker, then THIS
 * class's vtable and every member below. A real `PathLift` class does exist --
 * in ov002, `_ZN8PathLiftD1Ev` and friends at 0x020ef320..0x020eff18 -- so the
 * old name was matched across an OVERLAY BOUNDARY at a near-identical address,
 * not a coined one like MgBounceAndPounce was. Renamed to the fallback
 * `<ClassName>_Spawn` convention (compare daObjEmmLog_c_classInit): no game object
 * name is known for this minigame, unlike dScMgJump_c's own factory
 * dScMgJump_c_classInit.
 *
 * mPlayers is dMgJump3DMario_c[3], now named by the ROM's own RTTI and its
 * paired constructor/destructor helpers. The compiler therefore owns the
 * final array teardown after the still-explicit Model, mArray3 and mArray2
 * steps. mModel remains raw for now: typing only it would move its automatic
 * destruction after those two still-opaque arrays, opposite the ROM order.
 * The factory's `_ZN5ModelC1Ev(p + 0x5a14)` and sizeof(Model) == 0x50 still
 * fix the model's offset and extent without pretending the other element
 * type names are known.
 *
 * THE DESTRUCTOR IS DEFINED IN THE CLASS BODY, for the emission-order reason
 * spelled out at the definition below. No separate operator delete is needed:
 * dScMgD3DBase_c, the immediate base, provides one.
 *
 * SM64DS RTTI names the implementation dScMgJump2_c. The reconstructed factory
 * dScMgJump2_c_classInit (historical alias dScMgJump2_c_Spawn) installs this class's
 * cartridge vtable for the MG_JUMP2 registry profile.
 */
#ifndef DSCMGJUMP2_C_H
#define DSCMGJUMP2_C_H
#include "dScMgD3DBase_c.h"
#include "dMgJump3DMario_c.h"

extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void _ZN5ModelD1Ev(void *p);
extern "C" void func_ov006_020c6f3c(void);
extern "C" void func_ov006_020eed64(void);

struct dScMgJump2_c : dScMgD3DBase_c {
    /* DEFINED IN THE CLASS BODY, DELIBERATELY -- not a style choice.
       D1 sits at 0x020eebe8, BELOW D0 at 0x020eec9c. An out-of-line member
       definition makes mwccarm 2004/b56 emit the destructor group as
       D0-then-D1 plus a homeless D2, and a single translation unit covering
       the whole .text run 0x020eebe8..0x020efc08 cannot then be linked: the
       functions are not emitted in ROM address order. Defining it here emits
       D1 before D0, emits no D2 at all, and costs nothing else -- it is still
       the same body, and it is still not inlined into callers because a
       virtual destructor is always reached through the vtable.
       It does give up the key function: with this inline, the first virtual
       DECLARED but not defined here is OnYoshiTryEat below, which lives in
       src/minigames/d_s_mg_jump2.cpp, so _ZTV12dScMgJump2_c is still emitted
       by that one translation unit and by no other. */
    virtual ~dScMgJump2_c() {
        _ZN5ModelD1Ev((char *)this + 0x5a14);
        __destroy_arr(mArray3, 0x10, 0x24, (void *)func_ov006_020eed64);
        __destroy_arr(mArray2, 6, 0xf0, (void *)func_ov006_020c6f3c);
    }
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */

    /* 0x5004 -- the state callback, a pointer-to-member of this class, which
       is mwccarm's eight bytes exactly. Behavior calls through it every frame:
       `(this->**(State *)pad_5004)()`. Was "no matched access", which was only
       ever true of the files NAMED after this class -- the access was in
       func_ov006_020ef3e0 all along, and reads as this class's own now that
       the function carries its real name. Left as bytes rather than declared
       as the member pointer: naming it would fix a signature for every state
       function in the table, and none of them is recovered. */
    u8  pad_5004[0x8];   /* 0x5004 -- the state callback; see the block above */
    dMgJump3DMario_c mPlayers[3]; /* 0x500c -- RTTI-proven element type */
    u8  mArray2[0x5a0];   /* 0x5234 -- 6 * 0xf0,  elem dtor func_ov006_020c6f3c */
    u8  mArray3[0x240];   /* 0x57d4 -- 0x10 * 0x24, elem dtor func_ov006_020eed64 */
    u8  mModel[0x50];     /* 0x5a14 -- a real Model, raw bytes; see banner */
    s32 unk_5a64;         /* 0x5a64 */
    u8  pad_5a68[0x4];    /* 0x5a68 */
    u32 unk_5a6c;         /* 0x5a6c */
    u8  pad_5a70[0x8];    /* 0x5a70 -- no matched access */

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and none adds a field, so the size assert below is
       untouched. The destructor above stays the key function, so no
       translation unit starts emitting _ZTV12dScMgJump2_c because of these.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged. --- */
    s32 InitResources();      /* slot 0 -- src/_ZN12dScMgJump2_c13InitResourcesEv.cpp */
    s32 CleanupResources();   /* slot 3 -- ov006 0x020ef110 */
    s32 Behavior();           /* slot 6 -- ov006 0x020ef3e0 */
    s32 Render();             /* slot 9 -- ov006 0x020ef148 */
};

typedef char dScMgJump2_c_size_must_be_0x5a78[sizeof(dScMgJump2_c) == 0x5a78 ? 1 : -1];

#endif
