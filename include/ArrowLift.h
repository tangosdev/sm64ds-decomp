/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ArrowLift: 6 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ARROWLIFT_H
#define ARROWLIFT_H
#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

/* ROM identity versus compatibility spelling:
 *
 * The vtable whose compiler-facing symbol is `_ZTV9ArrowLift` has a 32-slot
 * dBgActor_c-shaped table, but its ABI preamble points at the cartridge's
 * `_ZTI15daObjWc_Obj02_c` (0x02113cac), whose name record says
 * `15daObjWc_Obj02_c` and whose sole base is dBgActor_c. `ArrowLift` is the
 * repository's readable compatibility name used by the existing function
 * symbols, not a claim about the original EAD RTTI spelling. Consequently a
 * per-function ArrowLift object can emit `_ZTI9ArrowLift`; that compiler-only
 * passenger must remain discarded by objisolate and is not evidence that this
 * spelling owns the ROM RTTI or is ready for whole-TU promotion.
 *
 * The ROM destructor proves that 0x000..0x31f is one dBgActor_c base: after
 * restoring _ZTV10dBgActor_c it destroys the base's dBgW_KcMbg at 0x124 and
 * Model at 0x0d4, then chains to dActor_c::~dActor_c. ArrowLift itself owns
 * only the eight-byte state tail below. */
struct ArrowLift : dBgActor_c {
    s32 mTravelDist;          /* 0x320 */
    u16 mStateTimer;          /* 0x324 */
    u8  mTriggered;           /* 0x326 */
    u8  mState;               /* 0x327 */

    /* Inline is load-bearing: when forced from the two destructor source
     * files, mwccarm emits the ROM's D1/D0 bodies without a homeless D2. */
    virtual ~ArrowLift() {}

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. Virtualness is inherited. */
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

#else

/* Flat compatibility view for the still-C factory. */
struct ArrowLift {
    u8  pad_000[0x320];
    s32 mTravelDist;          /* 0x320 */
    u16 mStateTimer;          /* 0x324 */
    u8  mTriggered;           /* 0x326 */
    u8  mState;               /* 0x327 */
};

#endif

typedef char ArrowLift_size_must_be_0x328[sizeof(struct ArrowLift) == 0x328 ? 1 : -1];

#endif
