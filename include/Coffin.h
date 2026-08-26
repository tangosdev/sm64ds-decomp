/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Coffin: 6 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef COFFIN_H
#define COFFIN_H
#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

/* ROM identity versus compatibility spelling:
 *
 * The vtable used by the readable `_ZN6Coffin...` function names points at
 * `_ZTI13daObjCasket_c` in the cartridge. Its RTTI record names
 * `daObjCasket_c` and gives dBgActor_c as its sole base. `Coffin` remains the
 * repository's readable compatibility spelling; compiler-emitted Coffin RTTI
 * is a per-function passenger that objisolate must discard.
 *
 * The destructor proves 0x000..0x31f is the dBgActor_c base: it destroys the
 * inherited dBgW_KcMbg at 0x124 and Model at 0x0d4, then chains to dActor_c.
 * The remaining fields are evidenced by the Coffin TU's state helpers. */
struct Coffin : dBgActor_c {
    s32 mState;             /* 0x320 */
    u16 mStateTimer;        /* 0x324 */
    s16 mAngleStep;         /* 0x326 */
    u16 mBehaviorTimer;     /* 0x328 */
    u16 unk_32a;            /* 0x32a */

    /* Inline is load-bearing: the two destructor sources force mwccarm to
     * emit the ROM's D1/D0 pair without creating a homeless D2. */
    virtual ~Coffin() {}

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. */
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

#else

/* Flat compatibility view for C translation units. */
struct Coffin {
    u8  pad_000[0x320];
    s32 mState;             /* 0x320 */
    u16 mStateTimer;        /* 0x324 */
    s16 mAngleStep;         /* 0x326 */
    u16 mBehaviorTimer;     /* 0x328 */
    u16 unk_32a;            /* 0x32a */
};

#endif

typedef char Coffin_size_must_be_0x32c[sizeof(struct Coffin) == 0x32c ? 1 : -1];

#endif
