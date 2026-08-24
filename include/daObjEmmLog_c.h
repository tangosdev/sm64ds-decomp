/* Derives from dBgActor_c: the factory is fBase_c::operator new(808), then
 * dBgActor_c::dBgActor_c(), then a single vtable store to this class's own
 * table (func_ov052_02111410, the Spawn). The destructor confirms the same
 * shape in reverse: one vtable store to this class's own table, then one to
 * _ZTV10dBgActor_c -- inlined, per include/dBgActor_c.h -- destroying
 * dBgW_KcMbg at 0x124 and Model at 0xd4 before chaining to dActor_c.
 * No new virtual is added (Kill is inherited unmodified from dBgActor_c), so
 * the out-of-line destructor is this class's key function.
 *
 * SIZE 0x328, the literal the factory passes to operator new: dBgActor_c ends
 * 0x320 and this class adds exactly two plain s32 fields with no constructor
 * or destructor calls of their own -- InitResources (func_ov052_02111348)
 * writes both, Behavior (func_ov052_021112ac) reads both. Field NAMES are
 * placeholders. */
#ifndef DAOBJEMMLOG_C_H
#define DAOBJEMMLOG_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjEmmLog_c : dBgActor_c {
    s32 mBasePosY;          /* 0x320 -- InitResources copies mPosY; Behavior computes mPosY = mBasePosY + sine * mBobAmplitude */
    s32 mBobAmplitude;      /* 0x324 -- 0x64000, or the spawn byte * 0xa000 */

    virtual ~daObjEmmLog_c();

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
    s32 unk_060;            /* 0x060 */
    u8  pad_064[0x2bc];
    s32 mBasePosY;          /* 0x320 -- InitResources copies mPosY; Behavior computes mPosY = mBasePosY + sine * mBobAmplitude */
    s32 mBobAmplitude;      /* 0x324 -- 0x64000, or the spawn byte * 0xa000 */
};

#endif /* __cplusplus */

#endif
