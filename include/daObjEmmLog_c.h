/* Derives from dBgActor_c: the factory is fBase_c::operator new(808), then
 * dBgActor_c::dBgActor_c(), then a single vtable store to this class's own
 * table (daObjEmmLog_c_classInit, ov052 0x02111410). The destructor confirms the same
 * shape in reverse: one vtable store to this class's own table, then one to
 * _ZTV10dBgActor_c -- inlined, per include/dBgActor_c.h -- destroying
 * dBgW_KcMbg at 0x124 and Model at 0xd4 before chaining to dActor_c.
 * No new virtual is added (Kill is inherited unmodified from dBgActor_c), so
 * the destructor -- declared first, and so the first virtual -- is this class's
 * key function. It is defined in the class body, not out of line; the reason is
 * on the declaration itself.
 *
 * SIZE 0x328, the literal the factory passes to operator new. dBgActor_c's
 * final named field ends at 0x31e and the base rounds to 0x320, so this class
 * reuses that tail padding for its spin angle before adding two s32 fields.
 * InitResources seeds all three; Behavior advances the angle and reads the
 * two words. */
#ifndef DAOBJEMMLOG_C_H
#define DAOBJEMMLOG_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjEmmLog_c : dBgActor_c {
    s16 mSpinAngle;         /* 0x31e -- seeded from mAngleX, += 0x200 per Behavior */
    s32 mBasePosY;          /* 0x320 -- InitResources copies mPosY; Behavior computes mPosY = mBasePosY + sine * mBobAmplitude */
    s32 mBobAmplitude;      /* 0x324 -- 0x64000, or the spawn byte * 0xa000 */

    /* Defined here, in the class body, and not out of line in the .cpp: the
     * cartridge keeps D1 (0x021111a0) BELOW D0 (0x021111e4), and an out-of-line
     * member definition makes mwccarm emit the D2/D1/D0 group in the order
     * D0-then-D1, which the whole-range link refuses (`licensed .text functions
     * are not emitted in ROM address order`). Written in the class body the group
     * comes out ROM-ascending. The body is genuinely empty -- every store and call
     * in the two ROM bodies is base and member destruction the compiler generates.
     * Slots 16 (D1) and 17 (D0). */
    virtual ~daObjEmmLog_c() {}

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

typedef char daObjEmmLog_c_size_must_be_0x328[sizeof(daObjEmmLog_c) == 0x328 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct daObjEmmLog_c {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x2ba];
    s16 mSpinAngle;         /* 0x31e */
    s32 mBasePosY;          /* 0x320 -- InitResources copies mPosY; Behavior computes mPosY = mBasePosY + sine * mBobAmplitude */
    s32 mBobAmplitude;      /* 0x324 -- 0x64000, or the spawn byte * 0xa000 */
};

#endif /* __cplusplus */

#endif
