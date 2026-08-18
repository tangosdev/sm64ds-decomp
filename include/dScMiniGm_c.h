#ifndef DSCMINIGM_C_H
#define DSCMINIGM_C_H
#include "dScene_c.h"

/* One of dScene_c's ten direct children (see the census in dScene_c.h) --
 * the minigame-select scene, per the ROM's own RTTI name. It had a FLAT
 * AUTO-GENERATED header before this pass, with no base clause: seven
 * resolvable vtable slots were blocked on it.
 *
 * BASE. dScene_c, confirmed independently two ways:
 *   - RTTI: dScMiniGm_c's __si_class_type_info (ov005:0x020c2448, name
 *     "dScMiniGm_c" at 0x020c2454) names its single base dScene_c
 *     (build/rtti.json).
 *   - Vtable: _ZTV11dScMiniGm_c (ov005:0x020c2490) is 18 slots, matching
 *     dScene_c's own 18 (tools/rtti_vtables.py --own dScMiniGm_c). It
 *     overrides exactly seven -- 0, 3, 6, 9, 12, 16, 17 -- every other slot
 *     is still whatever dScene_c's own table holds there.
 *
 * CONSTRUCTION. func_ov005_020c21ec is the factory: it opens with
 * `_ZN7fBase_cnwEj(0xb0)` -- fBase_c::operator new(0xb0). SIZE 0xb0 (176
 * bytes) therefore comes directly off the allocator call. The factory
 * writes only the vtable chain (fBase_c -> dScene_c inlined ->
 * data_ov005_020c2490) and two spawn-flag bits at fBase_c's own 0x13; it
 * constructs no nested sub-object, matching the plain-scalar field layout
 * below.
 *
 * MEMBERS. The old flat header put every field it found at or above 0x54,
 * which is consistent with dScene_c adding no fields of its own (its own
 * header proves that independently) -- so nothing here needed
 * re-attribution the way dScEntry_c's did. But the flat header was still
 * INCOMPLETE: reading every one of the six slot functions directly turns up
 * eight fields it missed (0x050, 0x058, 0x05c, 0x060, 0x064, 0x0a0, 0x0a4,
 * 0x0a8), on top of the six it had. All are read/written across at least
 * two of {InitResources, Behavior, Render}:
 *   0x050  unk_050 -- InitResources sets it to 0xb0 or 0 depending on
 *                     data_0209b304; Render reads it back for two
 *                     SubBg-offset calls (*(int*)(c+0x50) & 0x1ff)
 *   0x054  unk_054 -- u8; zeroed by InitResources, set to 1 by Behavior's
 *                     three countdown branches; Render's own local struct
 *                     independently types it as u8 at the same offset
 *   0x058  unk_058 -- set to data_0208a170 by InitResources; Render's local
 *                     struct independently confirms a 4-byte field here
 *   0x05c  unk_05c -- zeroed by InitResources; Render's local struct agrees
 *   0x060  unk_060 -- zeroed by InitResources
 *   0x064  unk_064 -- zeroed by InitResources
 *   0x08c  unk_08c -- Behavior increments and wraps it at 0x40
 *   0x090  unk_090 -- Behavior counts it down to 0
 *   0x094  unk_094 -- Behavior counts it down to 0
 *   0x098  unk_098 -- Behavior counts it down, gates SetAndStopColorFader
 *   0x09c  unk_09c -- Behavior increments and wraps it at 0x40
 *   0x0a0  unk_0a0 -- zeroed by InitResources
 *   0x0a4  unk_0a4 -- zeroed by InitResources
 *   0x0a8  unk_0a8 -- zeroed by InitResources
 *   0x0ac  unk_0ac -- u8; zeroed by InitResources, set 1 by Behavior on the
 *                     SetAndStopColorFader branch
 * 0x064..0x08c and 0x0ac..0xb0 are left as padding: no slot function
 * touches them, and unk_0ac (the last field either witness names) plus
 * 3 bytes of tail padding closes exactly on the 0xb0 allocation.
 *
 * VTABLE ORDER follows dScene_c's; dScMiniGm_c adds no new virtual. The
 * destructor pair is at slots 16/17, confirmed directly against the
 * _ZN8dScene_cD1Ev / _ZN8dScene_cD0Ev anchors.
 */
struct dScMiniGm_c : dScene_c {
    s32 unk_050;                 /* 0x050 */
    u8  unk_054;                 /* 0x054 */
    u8  pad_055[0x3];
    s32 unk_058;                 /* 0x058 */
    s32 unk_05c;                 /* 0x05c */
    s32 unk_060;                 /* 0x060 */
    s32 unk_064;                 /* 0x064 */
    u8  pad_068[0x24];  /* 0x068 -- opaque; untouched by any slot fn */
    s32 unk_08c;                 /* 0x08c */
    s32 unk_090;                 /* 0x090 */
    s32 unk_094;                 /* 0x094 */
    s32 unk_098;                 /* 0x098 */
    s32 unk_09c;                 /* 0x09c */
    s32 unk_0a0;                 /* 0x0a0 */
    s32 unk_0a4;                 /* 0x0a4 */
    s32 unk_0a8;                 /* 0x0a8 */
    u8  unk_0ac;                 /* 0x0ac */
    u8  pad_0ad[0x3];

    /* Declared first -- key function; see the family convention discussed
       in dScene_c.h. Never defined as a real method in any TU: both D1 and
       D0 are plain functions carrying their literal mangled name
       (src/_ZN11dScMiniGm_cD1Ev.c, src/_ZN11dScMiniGm_cD0Ev.c). */
    virtual ~dScMiniGm_c();                              /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

typedef char dScMiniGm_c_size_must_be_0xb0[sizeof(dScMiniGm_c) == 0xb0 ? 1 : -1];

#endif
