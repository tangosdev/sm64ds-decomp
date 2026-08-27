#ifndef CLOCKPAINTINGPENDULUM_H
#define CLOCKPAINTINGPENDULUM_H

#include "types.h"

#ifdef __cplusplus
#include "dActor_c.h"
#include "Model.h"
#endif

/* The swinging pendulum inside the clock painting -- a leaf dActor_c child at
 * ov013 0x021111a0..0x02111384, alongside its sibling ClockPaintingHandShort
 * (include/ClockPaintingHandShort.h, a separate class -- same painting, not a
 * base/derived relationship: neither RTTI record names the other).
 *
 * DERIVATION. tools/rtti_extract.py has the RTTI record at ov013 0x021120e0,
 * mangled "18daObjClockHuriko_c" -- the ROM struct name (`huriko` is the
 * ROM's own word for pendulum) -- with ONE base, dActor_c, at subobject
 * offset 0. It is a leaf: nothing in the image derives from it. No header
 * existed for either the ROM name or the coined one before this file --
 * include/decl_common.h's `extern int _ZTV18daObjClockHuriko_c[];` is a
 * stale, unresolved declaration (no symbols.txt entry backs it); the real
 * vtable symbol is _ZTV21ClockPaintingPendulum, ov013 0x02112128.
 *
 * VTABLE. _ZTV21ClockPaintingPendulum is 31 slots, the same count as
 * dActor_c's own table -- confirmed with `tools/rtti_vtables.py --own
 * daObjClockHuriko_c`, which also shows the destructor pair already migrated
 * under the coined name "ClockPaintingPendulum" (_ZN21ClockPaintingPendulumD1Ev
 * / D0Ev, ov013 0x021111a0 / 0x021111d0) by earlier work. This class
 * overrides four slots beyond the destructor:
 *
 *   0  InitResources      ov013 0x0211133c  (src/game/actors/ClockPaintingPendulum/_ZN21ClockPaintingPendulum13InitResourcesEv.cpp)
 *   3  CleanupResources   ov013 0x02111214  (src/_ZN21ClockPaintingPendulum16CleanupResourcesEv.c)
 *   6  Behavior           ov013 0x021112a8  (src/_ZN21ClockPaintingPendulum8BehaviorEv.c)
 *   9  Render             ov013 0x02111280  (src/game/actors/ClockPaintingPendulum/_ZN21ClockPaintingPendulum6RenderEv.cpp)
 *
 * (config/arm9/overlays/ov013/relocs.txt has no entries at the vtable's own
 * slot addresses because the whole vtable is one relocation-free literal
 * data blob in this overlay; the four function addresses above are exactly
 * what tools/rtti_vtables.py read out of that data, cross-checked against
 * config/arm9/overlays/ov013/symbols.txt's function addresses.) Slot 12
 * (OnPendingDestroy) is NOT overridden -- unlike Door (include/Door.h), which
 * overrides all five of InitResources/CleanupResources/Behavior/Render/
 * OnPendingDestroy, this class leaves slot 12 pointing at fBase_c's
 * implementation, so it is not part of this recovery.
 *
 * All four bytes still match; only the symbol NAMES were placeholders
 * (func_ov013_0211xxxx), now renamed. src/unnamed/ov013/0211/func_ov013_02111238.c, the small
 * helper both InitResources and Behavior call, is untouched -- it is not a
 * vtable slot and stays under its func_ name.
 *
 * NOT CONVERTED TO REAL METHODS BY THIS PASS, same idiom as include/Door.h
 * and src/game/actors/fBase_c/_ZN7fBase_c13InitResourcesEv.cpp: each of the four sources above is
 * declared here as a virtual override so the header documents the vtable
 * completely, but defined as a free function taking the object pointer
 * explicitly, never as a real `ClockPaintingPendulum::` method -- so nothing
 * about the bodies had to change to land the correct mangled symbol.
 *
 * SIZE. ClockPaintingPendulum_Spawn.c calls `_ZN7fBase_cnwEj(296)` -- 0x128 --
 * then _ZN8dActor_cC2Ev and _ZN5ModelC1Ev at +0xd4. dActor_c is 0xd0
 * (include/dActor_c.h) and Model is 0x50 (include/Model.h), so the embedded
 * Model runs 0xd4..0x124 (the same 4-byte alignment pad include/dBgActor_c.h
 * and include/Door.h both take before their own Model/ModelAnim members).
 * That leaves exactly 0x124..0x127 (4 bytes) as this class's own storage --
 * one s16, read and written as `*(short *)(c + 0x124)` in both
 * InitResources (set to 0x100) and Behavior (nudged by 8 per frame, compared
 * against +-0x10), plus 2 bytes of trailing padding. Same offset,
 * ClockPaintingHandShort.h's sibling field (`mHandIndex`, u8) confirms the
 * shape -- one small scalar right after the embedded Model.
 *
 * Field NAMES are placeholders and cannot change codegen. Offsets and widths
 * are observed.
 */

#ifdef __cplusplus

struct ClockPaintingPendulum : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by ClockPaintingPendulum_Spawn.c's own _ZN5ModelC1Ev call at
       +0xd4 -- a relocation the ROM build checks. */
    Model mModel;             /* 0x0d4 */

    s16 mAngSpeed;               /* 0x124 -- swing angle/phase, see SIZE above */
    u8  pad_126[0x2];

    /* --- vtable. Declared first, deliberately -- it is already the key
       function (see DERIVATION above): _ZN21ClockPaintingPendulumD1Ev.c /
       D0Ev.c define it as extern "C" free functions, never as a real
       `ClockPaintingPendulum::~ClockPaintingPendulum()`, so nothing here
       changes which TU the vtable is emitted from. --- */
    virtual ~ClockPaintingPendulum();

    /* --- overrides of inherited fBase_c slots dActor_c left untouched.
       Declared here purely so this header documents the vtable completely;
       each is DEFINED as a free function under its mangled symbol, not as a
       real ClockPaintingPendulum:: method -- see NOT CONVERTED above. --- */
    virtual s32 InitResources();          /* slot 0 */
    virtual s32 CleanupResources();       /* slot 3 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */
};

/* Holds the chain to the size ClockPaintingPendulum_Spawn.c's
   operator new(0x128) call evidences. A silently-added member anywhere
   fails this. */
typedef char ClockPaintingPendulum_size_must_be_0x128[sizeof(ClockPaintingPendulum) == 0x128 ? 1 : -1];

#endif /* __cplusplus */

#endif /* CLOCKPAINTINGPENDULUM_H */
