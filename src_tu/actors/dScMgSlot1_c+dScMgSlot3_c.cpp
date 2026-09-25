//cpp
/* SHADOW translation unit -- started by tools/tubuild.py create, then
 * reconciled by hand.  ov006/dScMgSlot1_c+dScMgSlot3_c.
 *
 * NOT ENROLLED, NOT CANONICAL. This file contributes nothing to the ROM build:
 * config/arm9/overlays/ov006/delinks.txt is untouched and the legacy
 * one-function shards still own every range. Manifest status is text-verified.
 *
 * WHAT IT CLAIMS: ROM ordinals 0..38 of the ROM's own 41-function translation
 * unit, .text 0x0210a8c0..0x0210c9e0, all 39 byte-matching. Ordinal 39,
 * _ZN12dScMgSlot1_c8BehaviorEv at 0x0210c9e0, has no delinks entry of any kind
 * -- its recovered source carries an honest NONMATCHING banner and tools/enroll.py
 * keeps it out of the ROM build, so dsd serves that range from raw ROM. A delink
 * entry cannot skip a range and resume, so 0..38 is the largest contiguous run
 * this unit can offer; ordinal 40 lies beyond the gap.
 *
 * The local shadow declarations below were carried from the legacy files by the
 * generator and then reconciled by hand where two of them disagreed -- see the
 * manifest's notes for the list. The generator's own unapplied alternates are
 * kept as // comments beneath each decision.
 *
 * FUNCTION ORDER IS ASCENDING ROM ORDER, and that is not a style choice. This
 * file sets #pragma defer_codegen off so that the optimisation-pragma brackets
 * below bind per function instead of being file-global last-wins. With codegen
 * no longer deferred mwccarm emits .text in SOURCE order rather than reversed,
 * so ascending source is what puts .text in ascending ROM order. Changing one
 * of the two without the other breaks the emitted order. */

/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources
 * of this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is
 * carried verbatim before its own member below, bracketed with `off` so it
 * cannot leak into later members (dropping it silently costs the pooled
 * cross-overlay tail-call -- a byte diff; see ShutterBob in ov014).
 * Any OTHER pragma is FILE-GLOBAL last-wins (opt_propagation,
 * optimize_for_size) and is still left out: carried into a merged TU it
 * would silently recompile every other member. Decide those by hand:
 *   func_ov006_0210af64: #pragma opt_strength_reduction off   [NOT carried -- review]
 *   func_ov006_0210af64: #pragma opt_loop_invariants off   [NOT carried -- review]
 *   _ZN12dScMgSlot3_c13OnYoshiTryEatEi: #pragma opt_strength_reduction off   [NOT carried -- review]
 *   _ZN12dScMgSlot3_c6RenderEv: #pragma opt_strength_reduction off   [NOT carried -- review]
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "dScMgSlot1_c.h"
#include "dScMgSlot3_c.h"
#include "dScMgBase_c.h"
#include "decl_common.h"
#include "private/ov006_slotgrid.h"
#include "types.h"

#pragma defer_codegen off
/* Codegen is deferred by default in mwccarm 2004/b56, which is why its
 * optimisation pragmas are otherwise file-global last-wins. With it off, the
 * positional brackets below bind to the functions they enclose -- and .text is
 * then emitted in SOURCE order, so this file is written in ASCENDING ROM order.
 */

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'G2' */
struct G2 { static void* GetBG1ScrPtr(); };

/* shadow namespace 'Sound' */
namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

/* shadow struct 'Obj' */
struct Obj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18(int a);
    virtual void v19();
    /* Reconciled from the two legacy spellings of this same shadow: the
       virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources) and
       the data-member body (dScMgSlot3_c::Behavior). Offsets are the ROM's
       absolute ones, so the leading pad is one word short to leave room for the
       vptr the virtuals put at offset 0. Only `idx` is reached by name; every
       other field in the legacy data spelling was already a raw offset on a
       char* cast, and stays one. */
    char pad0[0x5000 - 4];
    int idx;
};

/* shadow typedef 'T4fe4' */
typedef struct T4fe4 {
    char pad[0x4fe4];
    int vals[3];
} T4fe4;

/* shadow typedef 'void' */
typedef void (Obj::*ObjFn)();

/* shadow struct 'T' */
struct T {
    u8 pad[0x46c0];
    u8 grid[3][0x15];
    u8 idx[3];
    u8 pad2[6];
    u8 f708;
    u8 f709;
    u8 pad3;
    u8 count;
};

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot3_c8BehaviorEv, NOT applied:
// struct Obj {
//     char pad0[0x4f38];
//     int field_4f38;
//     char pad1[0x5000 - 0x4f3c];
//     int idx;
//     char pad2[0x5018 - 0x5004];
//     unsigned short h5018;
//     unsigned short h501a;
//     char pad3[0x503f - 0x501c];
//     unsigned char c503f;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot3_c13InitResourcesEv, NOT applied:
// struct Obj {
//     virtual void m00();
//     virtual void m04();
//     virtual void m08();
//     virtual void m0c();
//     virtual void m10();
//     virtual void m14();
//     virtual void m18();
//     virtual void m1c();
//     virtual void m20();
//     virtual void m24();
//     virtual void m28();
//     virtual void m2c();
//     virtual void m30();
//     virtual void m34();
//     virtual void m38();
//     virtual void m3c();
//     virtual void m40();
//     virtual void m44();
//     virtual void m48(int a);
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c180, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c1a8, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c208, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c218, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c234, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c278, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c2b0, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c2c0, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c2d4, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c354, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot1_c9betIcon_c6RenderEv, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c478, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot1_c19OnHitFromUnderneathEv, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot1_c15OnHitByMegaCharEv, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c500, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov006_0210c638, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot1_c13OnYoshiTryEatEi, NOT applied:
// struct Obj {
//     virtual void v00();
//     virtual void v01();
//     virtual void v02();
//     virtual void v03();
//     virtual void v04();
//     virtual void v05();
//     virtual void v06();
//     virtual void v07();
//     virtual void v08();
//     virtual void v09();
//     virtual void v10();
//     virtual void v11();
//     virtual void v12();
//     virtual void v13();
//     virtual void v14();
//     virtual void v15();
//     virtual void v16();
//     virtual void v17();
//     virtual void v18(int a);
//     virtual void v19();
//     /* Reconciled from two legacy spellings of this same shadow: the
//        virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
//        and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
//        ROM's absolute ones, so the leading pad is one word short to leave
//        room for the vptr the virtuals put at offset 0. Only `idx` is reached
//        by name; every other field in the legacy data spelling was already a
//        raw offset on a char* cast, and stays one. */
//     char pad0[0x5000 - 4];
//     int idx;
// };
// */

// /* TUBUILD CONFLICT -- alternate body of namespace 'Sound', from the legacy file for _ZN12dScMgSlot1_c8BehaviorEv, NOT applied:
// namespace Sound { void PlayBank2_2D(unsigned int); }
// */

// /* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN12dScMgSlot1_c13InitResourcesEv, NOT applied:
// struct Obj {
//     virtual void m00();
//     virtual void m04();
//     virtual void m08();
//     virtual void m0c();
//     virtual void m10();
//     virtual void m14();
//     virtual void m18();
//     virtual void m1c();
//     virtual void m20();
//     virtual void m24();
//     virtual void m28();
//     virtual void m2c();
//     virtual void m30();
//     virtual void m34();
//     virtual void m38();
//     virtual void m3c();
//     virtual void m40();
//     virtual void m44();
//     virtual void m48(int a);   /* vtable offset 0x48 */
// };
// */

extern "C" {
void SetBg1Offset(int a, int b);
int GetGameLanguage(void);
void* func_02054ea8(void);
unsigned int LoadCompressedFileAt(int fileID, void* target);
extern unsigned char data_0209d45c;
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern "C" void func_ov004_020b1ba0(void* c, int delta);
extern "C" void func_ov004_020b1b78(void* c, int val);
extern void func_02012790(int arg);
extern void func_ov004_020adb1c(int self);
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0deb[];
extern int data_ov006_0213e948[];
extern void func_ov006_0210ab08(char *c, int i);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern int Sound_PlayIfNotActive(int, int, int, int);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3);
void func_ov004_020b1e34(char *a0, int a1, int a2, int a3);
extern "C" char *data_ov006_0213e5ec[];
extern ObjFn data_ov006_02142bdc[];
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN3G2S13GetBG2CharPtrEv(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);
extern u8 data_0209d454;
extern int data_0208ee44;
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern int func_ov006_020c221c(char *t);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV12dScMgSlot3_c[];
extern void RenderOamMainScreen(void*, int, int, int, int);
extern void* data_ov006_0213e6a8;
extern void func_ov004_020b1b08(void *c);
extern void func_ov001_020ab3f0(void *c);
extern void *data_ov004_020beb68;
void _ZN3OAM9RenderSubEP7OamAttriiii(void* a, int b, int c, int d, int e);
void func_ov004_020af948(void* a, int b, int c, void* m);
extern void* data_ov006_0213e528[];
int TouchArea_Update(void *c, int x);
void func_ov006_0210c2d4(void *c);
void func_ov004_020b1b40(int x);
extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
extern int func_ov004_020ad8b8(void);
extern int data_ov006_0213e63c[][2];
extern void* data_ov006_0213e96c[];
extern unsigned char data_ov006_0213e4d8[];
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov006_0210c234(unsigned char* o);
extern int func_0202ec9c(void *fader, int a);
extern void func_ov004_020ad79c(int a, int b);
extern void func_ov004_020ae274(void *c);
extern void func_ov006_0210c180(void *o);
extern void func_ov006_0210c1a8(void *o);
extern void func_ov006_0210c218(void *o, s16 x, s16 y);
extern void func_ov006_0210c278(void *o);
extern void func_ov006_0210c2c0(void *o, int v);
extern int func_ov006_0210c500(void *self);
extern int data_ov006_0213e600[];
extern u8 data_ov006_0213e4d8[];
extern s16 data_ov006_0213e654[][2];
extern s16 data_ov006_0213e656[][2];
extern s16 data_ov006_0213e4f8[][2];
extern s16 data_ov006_0213e4fa[][2];
extern struct FaderBrightness data_0209f61c;
extern void func_ov004_020af770(void* a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
/* CONTROL: file-global pragma removed for this measurement */
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern char *_ZN2G212GetBG3ScrPtrEv(void);
extern char *_ZN3G2S13GetBG0CharPtrEv(void);
extern char *_ZN3G2S12GetBG0ScrPtrEv(void);
extern char *_ZN3G2S12GetBG1ScrPtrEv(void);
extern char *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
/* TUBUILD CONFLICT -- alternate declaration of DecIfAbove0_Byte, from the legacy file for func_ov006_0210ac3c, NOT applied: extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p); */
/* TUBUILD CONFLICT -- alternate declaration of DecIfAbove0_Byte, from the legacy file for func_ov006_0210adac, NOT applied: extern unsigned char DecIfAbove0_Byte(unsigned char *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012790, from the legacy file for _ZN12dScMgSlot3_c13OnYoshiTryEatEi, NOT applied: extern void func_02012790(unsigned int id); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for _ZN12dScMgSlot3_c8BehaviorEv, NOT applied: extern "C" int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of LoadCompressedFileAt, from the legacy file for _ZN12dScMgSlot3_c13InitResourcesEv, NOT applied: extern u32 LoadCompressedFileAt(u16 fileID, void *target); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209d45c, from the legacy file for _ZN12dScMgSlot3_c13InitResourcesEv, NOT applied: extern u8 data_0209d45c; */
/* TUBUILD CONFLICT -- alternate declaration of Hud_RenderSprite, from the legacy file for _ZN12dScMgSlot1_c6RenderEv, NOT applied: extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN12dScMgSlot1_c6RenderEv, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of DecIfAbove0_Byte, from the legacy file for _ZN12dScMgSlot1_c8BehaviorEv, NOT applied: extern u8 DecIfAbove0_Byte(u8 *p); */
/* TUBUILD CONFLICT -- alternate declaration of Sound_PlayIfNotActive, from the legacy file for _ZN12dScMgSlot1_c8BehaviorEv, NOT applied: extern int Sound_PlayIfNotActive(int handle, int a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012790, from the legacy file for _ZN12dScMgSlot1_c8BehaviorEv, NOT applied: extern unsigned int func_02012790(unsigned int a); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov004_020adb1c, from the legacy file for _ZN12dScMgSlot1_c8BehaviorEv, NOT applied: extern void func_ov004_020adb1c(int a); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_0210c2d4, from the legacy file for _ZN12dScMgSlot1_c8BehaviorEv, NOT applied: extern void func_ov006_0210c2d4(void *o); */
/* TUBUILD CONFLICT -- alternate declaration of LoadCompressedFileAt, from the legacy file for _ZN12dScMgSlot1_c13InitResourcesEv, NOT applied: extern u32 LoadCompressedFileAt(int fileID, void *target); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G212GetBG2ScrPtrEv, from the legacy file for _ZN12dScMgSlot1_c13InitResourcesEv, NOT applied: extern char *_ZN2G212GetBG2ScrPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2S13GetBG2CharPtrEv, from the legacy file for _ZN12dScMgSlot1_c13InitResourcesEv, NOT applied: extern char *_ZN3G2S13GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2S12GetBG2ScrPtrEv, from the legacy file for _ZN12dScMgSlot1_c13InitResourcesEv, NOT applied: extern char *_ZN3G2S12GetBG2ScrPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN12dScMgSlot1_c13InitResourcesEv, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209d454, from the legacy file for _ZN12dScMgSlot1_c13InitResourcesEv, NOT applied: extern unsigned char data_0209d454; */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 40 and 39 are DELIBERATELY NOT DEFINED HERE.                   */
/*                                                                            */
/* [39] _ZN12dScMgSlot1_c8BehaviorEv 0x0210c9e0 has no delinks entry at all in */
/* config/arm9/overlays/ov006/delinks.txt: its recovered source carries an     */
/* honest NONMATCHING banner and enroll.py keeps it out of the ROM build, so   */
/* dsd serves that range from raw ROM. A delink entry cannot skip a range and  */
/* resume, so no single claim can reach past it. [40] lies beyond it.          */
/*                                                                            */
/* This unit therefore claims ordinals 0..38, .text 0x0210a8c0..0x0210c9e0.    */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* ROM ordinals 37..20 -- src/actors/dScMgSlot1_c.cpp, 0x0210c180..0x0210c6c0 */
/* -------------------------------------------------------------------------- */
// NOTE: tools/tubuild.py create emitted this legacy file once per ROM ordinal
// (37 down to 20). It is ONE already-promoted 18-function translation unit,
// src/actors/dScMgSlot1_c.cpp, whose own internal order is already ROM-descending,
// so the 17 duplicate pastes were removed and a single copy kept in place.
/* dScMgSlot1_c -- ov006, intact translation unit.
 *
 * Licensed .text range 0x0210c180..0x0210c6c0, 18 functions, compiled by
 * mwccarm 2004/b56 and byte-verified against the ROM.
 *
 * This is a SUB-RANGE of a larger original TU. The ROM's own unit spans
 * 0x0210a8c0..0x0210d6b8 and holds dScMgSlot1_c and dScMgSlot3_c together
 * (41 functions); see this TU's manifest boundary_evidence for how that
 * boundary was derived and for the six functions that keep the full range
 * from being claimed. The same truncate-at-the-first-unmatched-function
 * shape is what src/actors/dScMgCurling2_c.cpp and
 * src/minigames/d_s_mg_hanachan.cpp already do in this overlay.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function, in the reverse of source order, so the
 * highest-address ROM function is written first.
 *
 * Folded from the 18 one-function legacy sources that used to carry these
 * symbols; those files are deleted by this change, so they are named here
 * by symbol rather than by a path that no longer resolves (ROM order):
 *   0x0210c180  func_ov006_0210c180
 *   0x0210c1a8  func_ov006_0210c1a8
 *   0x0210c208  func_ov006_0210c208
 *   0x0210c218  func_ov006_0210c218
 *   0x0210c234  func_ov006_0210c234
 *   0x0210c278  func_ov006_0210c278
 *   0x0210c2b0  func_ov006_0210c2b0
 *   0x0210c2c0  func_ov006_0210c2c0
 *   0x0210c2d4  func_ov006_0210c2d4
 *   0x0210c354  func_ov006_0210c354
 *   0x0210c374  _ZN12dScMgSlot1_c9betIcon_c6RenderEv
 *   0x0210c410  _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
 *   0x0210c478  func_ov006_0210c478
 *   0x0210c4b8  _ZN12dScMgSlot1_c19OnHitFromUnderneathEv
 *   0x0210c4dc  _ZN12dScMgSlot1_c15OnHitByMegaCharEv
 *   0x0210c500  func_ov006_0210c500
 *   0x0210c638  func_ov006_0210c638
 *   0x0210c674  _ZN12dScMgSlot1_c13OnYoshiTryEatEi
 */
/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'G2' */
/* shadow namespace 'Sound' */
/* shadow struct 'Obj' */
/* shadow typedef 'T4fe4' */
/* shadow typedef 'void' */
/* shadow struct 'T' */
/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN12dScMgSlot1_cD1Ev, 0x0210a8c0, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_cD1Ev
/* ~dScMgSlot1_c() (D1, complete-object destructor). The empty body is the
   original ownership model: the compiler destroys the typed mBetIcon member,
   emitting betIcon_c's then dThIcon_c's vtable writes, before chaining to the
   dScMgBase_c base destructor. */
dScMgSlot1_c::~dScMgSlot1_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN12dScMgSlot1_cD0Ev, 0x0210a900, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_cD0Ev
/* Deleting destructor for the same compiler-owned member teardown documented
   by src/_ZN12dScMgSlot1_cD1Ev.cpp. dScMgBase_c's own operator delete
   (its immediate base) covers the Memory::Deallocate call the
   pre-migration source made explicitly; no per-class copy needed, same
   precedent as every other dScMgBase_c leaf's D0. The tree-wide
   OnYoshiTryEat mislabel on this class landed HERE (its source comment
   said "recovered name: dScMgSlot1_c_OnYoshiTryEat") rather than on the
   real OnYoshiTryEat-shaped slot 18 helper -- this body is the textbook D0
   shape (member teardown, base D2, Deallocate), not gameplay logic.

   D0 has no source text of its own: mwccarm 2004/b56 emits both D1 and D0 from
   the single out-of-line ~dScMgSlot1_c() definition at ROM ordinal 0 below. The
   marker above is parked at D0's own ROM ordinal over this note. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN12dScMgSlot3_cD1Ev, 0x0210a954, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_cD1Ev
/* recovered: real C++ destructor. The one explicit call reproduces the
   ROM's own recovered body (func_ov006_0210a954, pre-migration): destroy
   the 0x4f38 table, addressed by raw offset from `this` because the header
   keeps it opaque. Everything after -- own vtable store, mSysTracker
   destruction, chain to ~dScMgBase_c() -- is the compiler's own inlining
   of dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note). */
dScMgSlot3_c::~dScMgSlot3_c()
{
    func_ov006_020c21e4((char *)this + 0x4f38);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN12dScMgSlot3_cD0Ev, 0x0210a9a8, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_cD0Ev
/* D0, the deleting destructor, has NO source text of its own: mwccarm 2004/b56
   emits both D1 and D0 from the single out-of-line ~dScMgSlot3_c() definition at
   ROM ordinal 2 below, and a second textual definition of it is a compile error.
   The marker above is parked at D0's own ROM ordinal, over this note, so that the
   member is marked without its fragment running on into a neighbour. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN12dScMgSlot3_c25OnAimedAtWithEggReturnVecEv, 0x0210aa10, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_c25OnAimedAtWithEggReturnVecEv
// recovered name: dScMgSlot3_c_OnAimedAtWithEggReturnVec
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method */
/* dScMgSlot3_c::OnAimedAtWithEggReturnVec - recovered from vtable slot identity.
   The name is dScMgBase_c's, transplanted down the hierarchy; the `ReturnVec`
   half of it is refuted in include/dScMgBase_c.h's slot-30 block, and this
   body is the arity witness cited there.  The read-modify-write below clobbers
   r1 before the tail call, so the base cannot be taking a second argument. */
void dScMgSlot3_c::OnAimedAtWithEggReturnVec()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1118;
    ((dScMgBase_c *)a)->dScMgBase_c::OnAimedAtWithEggReturnVec();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12dScMgSlot3_c16OnAimedAtWithEggEv, 0x0210aa3c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_c16OnAimedAtWithEggEv
// recovered name: dScMgSlot3_c_OnAimedAtWithEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
/* dScMgSlot3_c::OnAimedAtWithEgg - recovered from vtable slot identity.

   This body is the third and strongest witness that slot 29 takes no
   explicit parameter.  It CLOBBERS r1 -- the masked read-modify-write of the
   sub display-control register below compiles to `ldrh r1,[r2]; and; orr;
   strh` -- and only then tail-branches into the base with `bx ip`.  A second
   argument passed in r1 would reach dScMgBase_c::OnAimedAtWithEgg as a
   display-control word, so the base cannot be reading one. */
int dScMgSlot3_c::OnAimedAtWithEgg()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1000;
    ((dScMgBase_c *)a)->dScMgBase_c::OnAimedAtWithEgg();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN12dScMgSlot3_c9Virtual80Ev, 0x0210aa60, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_c9Virtual80Ev
// recovered name: dScMgSlot3_c_AfterClsn  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method */
/* dScMgSlot3_c::Virtual80 - slot 32, the only override of it in the family.

   Not an AfterClsn: the base slot has no ROM name at all, and the one the
   recovery pass borrowed belongs to dPathLiftActor_c, two forks away.  See the
   slot-32 block in include/dScMgBase_c.h.

   The base's body verbatim -- main BG1CNT reduced to 0x1000, scroll reset, BG1
   cleared from the main BG-enable shadow, this class's own language table and
   the shared screen map -- and then one more write, leaving BG1CNT at 0x1118
   instead of 0x1000: the same layer, pointed at this minigame's own character
   and screen base blocks. */
void dScMgSlot3_c::Virtual80()
{
    int idx;

    *(volatile unsigned short*)0x400000a &= ~3;
    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short*)0x400000a &= ~0x40;

    SetBg1Offset(0, 0);

    data_0209d45c &= ~2;

    idx = GetGameLanguage();
    LoadCompressedFileAt(data_ov006_0213e614[idx], func_02054ea8());
    LoadCompressedFileAt(0x67, G2::GetBG1ScrPtr());

    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1118;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_0210ab08, 0x0210ab08, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210ab08
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210ab08(char *c, int idx);
void func_ov006_0210ab08(char *c, int idx){
    int d = (*(int*)(c + idx*4 + 0x4fe4) >> 12) / 80;
    int i;
    char *dst = c + idx*3;
    char *src = c + idx*5;
    for (i = 0; i < 3; i++) {
        *(unsigned char*)(dst + 0x5031) = *(unsigned char*)(src + d + 0x501c);
        d = (d + 1) % *(unsigned char*)(c + 0x503a);
        dst += 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_0210ab90, 0x0210ab90, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210ab90
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210ab90(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_0210ab94, 0x0210ab94, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210ab94
extern "C" {  /* .c-derived member: C linkage for the whole block */

void func_ov006_0210ab94(char* c){
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        *(int*)(c + 0x5000) = 7;
        func_ov004_020b0a54(0x12);
        *(int*)(c + 0x5004) = 0;
    }
    if (*(unsigned char*)(c + 0x503e) >= 0x3c) return;
    {
        int* a = (int*)(((int)c + 0x4ff4));
        int* b = (int*)(((int)c + 0x4ff8));
        int* d = (int*)(((int)c + 0x4ffc));
        *a = *a + 0x10000;
        *b = *b - 0x10000;
        *d = *d + 0x10000;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_0210ac38, 0x0210ac38, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210ac38
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210ac38(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_0210ac3c, 0x0210ac3c, size 0x170 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210ac3c
extern "C" void func_ov006_0210ac3c(char* c)
{
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        if (*(int*)(c + 0x5000) == 3) {
            if (*(int*)(c + 0xb4) < 0x270f)
                *(int*)(((int)c + 0xb4)) += 1;
            if (*(int*)(c + 0xb4) > *(int*)(c + 0xb8))
                *(int*)(c + 0xb8) = *(int*)(c + 0xb4);
            ((Obj*)c)->v18(4);
        } else {
            if (*(int*)(c + 0xa8) <= 0) {
                *(unsigned char*)(c + 0x503e) = 0x78;
                *(int*)(c + 0x5000) = 6;
            } else {
                ((Obj*)c)->v18(5);
            }
        }
    }

    if (*(int*)(c + 0x5000) == 3) {
        if (*(unsigned char*)(c + 0x503e) != 0x14)
            return;
        if (*(int*)(c + 0x5014) <= 0)
            return;
        *(int*)(((int)c + 0x5014)) -= 1;
        func_ov004_020b1ba0(c, 1);
        Sound::PlayBank2_2D(0x149);
        *(unsigned char*)(((int)c + 0x503e)) += 5;
        return;
    }

    if (*(int*)(c + 0x5000) != 4)
        return;

    {
        unsigned char b = *(unsigned char*)(c + 0x503e);
        if (b != 0x14 && b != 0x28 && b != 0x3c)
            return;
    }

    func_ov004_020b1b78(c, 1);
    Sound::PlayBank2_2D(0x14a);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_0210adac, 0x0210adac, size 0x1b8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210adac
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210adac(char *c)
{
    unsigned char buf[3];
    int i;
    int total;
    int arg;
    char *self;

    if (DecIfAbove0_Byte((unsigned char *)(c + 0x503e)) != 0)
        return;

    self = c;
    for (i = 0; i < 3; i++) {
        int j;
        buf[i] = *(unsigned char *)(self + 0x5031 + i);
        j = 1;
        do {
            int rem = (i + (((int *)(self + 0x4fe4))[j] >> 12) / 80) % *(unsigned char *)(self + 0x503a);
            if (buf[i] != ((SlotGrid *)self)->sym[j][rem]) {
                buf[i] = 5;
                break;
            }
        } while (++j < 3);
        self = (char *)((unsigned)self | (unsigned)(i - i));
    }

    *(int *)(c + 0x5010) = -1;
    total = 0;
    {
        int k;
        for (k = 0; k < 3; k++) {
            if (buf[k] == *(unsigned char *)(c + 0x503b)) {
                if (k == 1) {
                    *(int *)(c + 0x5010) = k;
                    total += 6;
                } else if (k == 0 || k == 2) {
                    *(int *)(c + 0x5010) = k;
                    total += 3;
                }
                break;
            }
        }
    }

    arg = *(int *)(c + 0xb4);
    if (total > 0) {
        *(int *)(c + 0x5014) = total;
        *(int *)(c + 0x5000) = 3;
        func_02012790(0x26);
        *(unsigned char *)(c + 0x503e) = 0x28;
        arg += 1;
    } else {
        *(int *)(c + 0x5000) = 4;
        func_02012790(0xe);
        *(unsigned char *)(c + 0x503e) = 0x50;
    }
    func_ov004_020adb1c(arg);

    *(unsigned char *)(c + 0xc3) = 0;
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off
/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_0210af64, 0x0210af64, size 0x298 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210af64
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210af64(char *c)
{
  u8 idx;
  int ok;
  int off;
  int i;
  int new_var;
  int *tab;
  idx = data_020a0e40[0];
  ok = 0;
  off = idx * 4;
  new_var = 1;
  if (data_020a0de8[idx * 4])
  {
    if (data_020a0de9[off])
    {
      ok = 1;
    }
  }
  if (ok != 0)
  {
    unsigned int v = data_020a0deb[idx * 4];
    if ((v >= 0x40) && (v < 0x58))
    {
      if ((*((u8 *) (c + 0x502b))) == new_var)
      {
        *((u8 *) (c + 0x502b)) = 0;
      }
    }
    else
      if ((v >= 0x58) && (v < 0x70))
    {
      if ((*((u8 *) (c + 0x502c))) == 1)
      {
        *((u8 *) (c + 0x502c)) = 0;
      }
    }
    else
      if ((v >= 0x70) && (v < 0x88))
    {
      if ((*((u8 *) (c + 0x502d))) == new_var)
      {
        *((u8 *) (c + 0x502d)) = 0;
      }
    }
  }
  i = 0;
  for (; i < 3; i++)
  {
    if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x502e))))))) == 0)
    {
      int *p = (int *) (c + (i * 4));
      int val;
      int q;
      int q2;
      u8 *dir;
      tab = data_ov006_0213e948;
      p = (int *) ((int) (((long long) ((int) (((int) p) + 0x4fe4)))));
      val = *p;
      ;
      q = (val >> 12) / 0x50;
      if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x5040))))))) == 1)
      {
        *p = val - tab[((*((u8 *) (c + 0x503d))) * 3) + i];
        if ((*p) < 0)
        {
          *p += *((int *) (c + 0x4ff0));
        }
      }
      else
      {
        *p = val + tab[((*((u8 *) (c + 0x503d))) * 3) + i];
        if ((*p) >= (*((int *) (c + 0x4ff0))))
        {
          *p -= *((int *) (c + 0x4ff0));
        }
      }
      if ((*((u8 *) ((c + i) + 0x502b))) == 0)
      {
        q2 = ((*p) >> 12) / 0x50;
        if (q2 != q)
        {
          if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x5040))))))) == new_var)
          {
            *p = (q * 0x50) << 12;
          }
          else
          {
            *p = (q2 * 0x50) << 12;
          }
          func_ov006_0210ab08(c, i);
          *((u8 *) ((int) (((long long) ((int) ((c + i) + 0x502e)))))) = 1;
          *((u8 *) ((int) (((long long) ((int) (c + 0x503d)))))) += new_var;
          *((u8 *) (c + 0x503f)) = 0;
          _ZN5Sound12PlayBank2_2DEj(0x1a9);
        }
      }
    }
  }

  *((int *) (c + 0x5008)) = Sound_PlayIfNotActive(*((int *) (c + 0x5008)), 2, 0x1a8, 0);
  if ((*((u8 *) (c + 0x503d))) >= 3u)
  {
    *((u8 *) (c + 0x503e)) = 0x1e;
    *((int *) (c + 0x5000)) = 2;
  }
}
}

#pragma opt_loop_invariants on
#pragma opt_strength_reduction on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_0210b1fc, 0x0210b1fc, size 0x118 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210b1fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020c1f04(char *c, int *src);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
void func_ov006_020c1ef8(int *p);
void FreeGfxSlotsById(int arg);

void func_ov006_0210b1fc(char *p)
{
    if (*(int *)(p + 0x500c) == 0x50) {
        func_ov006_020c1f04(p + 0x4f38, 0);
        func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    }

    if (*(int *)(p + 0x500c) > 0) {
        *(int *)(((int)p + 0x500c)) -= 1;
        if (*(int *)(p + 0x500c) != 0)
            return;
        *(unsigned char *)(p + 0x503b) = *(unsigned char *)(p + 0x503c);
        func_ov006_020c1ef8((int *)(p + 0x4f38));
        return;
    }

    *(unsigned char *)(((int)p + 0x503e)) -= 1;
    if (*(unsigned char *)(p + 0x503e) != 0)
        return;

    FreeGfxSlotsById(0x1d);

    {
        int *q = (int *)(p + 0x5000);
        if (*(unsigned char *)(p + 0xc4) == 0) {
            *(unsigned char *)(p + 0xc3) = 1;
            *(unsigned char *)(p + 0xc4) = 1;
            *(short *)(p + 0xc0) = 0;
        }
        *q += 1;
    }
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN12dScMgSlot3_c13OnYoshiTryEatEi, 0x0210b314, size 0x334 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_c13OnYoshiTryEatEi
// recovered name: dScMgSlot3_c_OnYoshiTryEat_0210b314
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgSlot3_c::OnYoshiTryEat(int mode)
{
    char *c = (char *)this;

    if (mode == 3 || mode == 0x12) {
        *(int *)(c + 0xa8) = 0xc;
        *(int *)(c + 0xac) = *(int *)(c + 0xa8);
        *(int *)(c + 0xbc) = *(int *)(c + 0x5004);
        if (*(u32 *)(c + 0xbc) > 0x270e) {
            *(int *)(c + 0xbc) = 0x270e;
        }
        *(int *)(c + 0xb4) = 0;
        func_ov004_020adb1c(*(int *)(c + 0xb4));
        func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    } else if (mode == 4) {
        if (*(int *)(c + 0x5004) < 5) {
            *(int *)(int)(c + 0x5004) += 1;
        }
        *(int *)(int)(c + 0xbc) += 1;
        if (*(u32 *)(c + 0xbc) > 0x270e) {
            *(int *)(c + 0xbc) = 0x270e;
        }
    }

    if (mode == 3 || mode == 0x12 || mode == 4) {
        int v;
        int v2;
        int i;
        int j;
        u8 k;
        int m;

        v = *(int *)(c + 0x5004);
        if (v <= 0) {
            *(u8 *)(c + 0x503a) = 3;
        } else if (v <= 2) {
            *(u8 *)(c + 0x503a) = 4;
        } else {
            *(u8 *)(c + 0x503a) = 5;
        }

        v2 = *(int *)(c + 0x5004);
        if (v2 <= 0 || v2 == 1 || v2 == 3) {
            *(u8 *)(c + 0x5040) = 0;
            *(u8 *)(c + 0x5041) = 1;
            *(u8 *)(c + 0x5042) = 0;
        } else if (v2 == 2 || v2 == 4) {
            *(u8 *)(c + 0x5040) = 1;
            *(u8 *)(c + 0x5041) = 0;
            *(u8 *)(c + 0x5042) = 1;
        } else {
            for (i = 0; i < 3; i++) {
                if ((u8)(((u32)RandomIntInternal(&data_0209e650) >> 16) & 1)) {
                    *(u8 *)(c + i + 0x5040) = 1;
                } else {
                    *(u8 *)(c + i + 0x5040) = 0;
                }
            }
        }

        *(u8 *)(c + 0x503c) = ((u32)RandomIntInternal(&data_0209e650) >> 16) % *(u8 *)(c + 0x503a);
        if (mode != 4) {
            *(int *)(c + 0x500c) = 0;
            *(u8 *)(c + 0x503b) = *(u8 *)(c + 0x503c);
        } else {
            *(int *)(c + 0x500c) = 0x50;
            func_02012790(0x1aa);
        }

        {
            char *w = c;
            for (j = 0; j < 3; j++, w += 5) {
                for (k = 0; k < *(u8 *)(c + 0x503a); k++) {
                    if (j == 1) {
                        *(u8 *)(c + k + 0x5021) = *(u8 *)(c + 0x503a) - k - 1;
                    } else {
                        *(u8 *)(w + k + 0x501c) = k;
                    }
                }
            }
        }
        *(int *)(c + 0x4ff0) = (*(u8 *)(c + 0x503a) * 0x50) << 12;
    }

    {
        int j2;
        int k2;
        int m2;
        char *w2 = c;
        for (j2 = 0; j2 < 3; j2++, w2 += 3) {
            *(u8 *)(c + j2 + 0x502b) = 1;
            *(u8 *)(c + j2 + 0x502e) = 0;
            for (k2 = 0; k2 < 3; k2++) {
                *(u8 *)(w2 + k2 + 0x5031) = *(u8 *)(c + 0x503a);
            }
        }
        for (m2 = 0; m2 < 3; m2++) {
            *(int *)(c + m2 * 4 + 0x4ff4) = 0;
        }
        *(u8 *)(c + 0x503d) = 0;
        *(u8 *)(c + 0x503e) = 0x3c;
        *(int *)(c + 0x5000) = 0;
        *(int *)(c + 0x5010) = -1;
        *(int *)(c + 0x5014) = 0;
    }
}

#pragma opt_strength_reduction on

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN12dScMgSlot3_c6RenderEv, 0x0210b648, size 0x668 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_c6RenderEv
/* dScMgSlot3_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the second of the two slots where this class's
 * table differs from dScMgSingle3DBase_c's. The old file's `recovered name:`
 * comment agreed, and here it is right.
 *
 * Draws the three reels -- the win-line pass at state 6 and the scrolling pass
 * otherwise -- then the payout markers, the dMeter_c, and the two swinging lamps whose
 * angles are mLamp1Angle/mLamp2Angle.
 *
 * The pragma is load-bearing, not tidying; the `(int)` launder on the two indexed
 * reads at 0x4fe4/0x4ff4 is there for the same reason. Only the receiver changed:
 * `self` is `this` now instead of a char* parameter cast back to the class. */
s32 dScMgSlot3_c::Render()
{
    char *c = (char *)this;
    struct dScMgSlot3_c *self = this;
    int i, j;

    func_ov006_020c1eb4(c + 0x4660);
    func_ov006_020c201c(c + 0x4f38);

    if (self->mReelDrawY > 0) {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->mWinSymbol * 3 + i], 0x80, self->mReelDrawY + 0x10, -1, 2, 0x1000, 0x1000, 0, 1);
        }
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->unk_503c * 3 + i], 0x80, self->mReelDrawY + 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    } else {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->mWinSymbol * 3 + i], 0x80, 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    }

    if (self->mState != 7) {
        if (self->mState == 6) {
            int i2, j2;
            char *p = c;
            for (i2 = 0; i2 < 3; i2++) {
                int y;
                for (j2 = 0, y = 0x30; j2 < 3; j2++) {
                    Hud_RenderSprite(data_ov006_0213e9a4[*(u8 *)(p + j2 + 0x5031) * 3 + i2],
                                        y - (*(int *)((int)c + i2 * 4 + 0x4ff4) >> 12), 0x60, -1, -1);
                    y += 0x50;
                }
                p += 3;
            }
        } else {
            char *p = c;
            int row, rem;
            int i2, j2;
            for (i2 = 0; i2 < 3; i2++) {
                int a = *(int *)(((int)c + i2 * 4 + 0x4fe4)) >> 12;
                int y;
                row = a / 0x50;
                rem = a % 0x50;
                for (j2 = 0, y = 0x30; j2 < 4; j2++) {
                    Hud_RenderSprite(data_ov006_0213e9a4[*(u8 *)(p + row + 0x501c) * 3 + i2],
                                        y - rem, 0x60, -1, -1);
                    row = (row + 1) % self->mStripLength;
                    y += 0x50;
                }
                p += 5;
            }
        }
    }

    if (self->unk_503d < 3 && (self->mFrameCounter & 0x20)) {
        int slot = 3;
        int ok = 1;
        int i3, j3;
        char *p = c;
        for (i3 = 0; i3 < 3; i3++) {
            if (*(u8 *)(c + i3 + 0x502e) != 0) {
                if (slot < 3) {
                    if (self->mWinSymbol != *(u8 *)(p + slot + 0x5031))
                        ok = 0;
                } else {
                    u8 t = self->mWinSymbol;
                    for (j3 = 0; j3 < 3; j3++) {
                        if (t == *(u8 *)(p + j3 + 0x5031)) {
                            slot = j3;
                            break;
                        }
                        if (j3 == 2)
                            ok = 0;
                    }
                }
            }
            if (ok == 0)
                break;
            p += 3;
        }
        if (ok != 0 && slot < 3) {
            int count = 0;
            int i4;
            int y = 0x4c;
            for (i4 = 0; i4 < 3; i4++) {
                if (*(u8 *)(c + i4 + 0x502e) != 0) {
                    Hud_RenderSprite(&data_ov006_0213e5dc, slot * 0x50 + 0x30, y, -1, -1);
                    count++;
                }
                y += 0x18;
            }
            if ((self->mFrameCounter & 0x3f) == 0x20) {
                _ZN5Sound12PlayBank2_2DEj(count > 1 ? 0x1ac : 0x1ab);
            }
        }
    }

    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);

    if (self->mState == 3 && self->mWinColumn >= 0) {
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 8), self->mWinColumn * 0x50 + 0x20, 0x28, 0);
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 0x34), self->mWinColumn * 0x50 + 0x30, 0x28, 0);
        if (self->mWinColumn == 1) {
            func_ov004_020b2444(self->mWinColumn * 0x50 + 0x40, 0x28, 6, 0, 0, 0, 0x14);
        } else {
            func_ov004_020b2444(self->mWinColumn * 0x50 + 0x40, 0x28, 3, 0, 0, 0, 0x14);
        }
    } else if (self->mState == 4 && self->mWinColumn < 0) {
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 8), 0x70, 0x28, 0);
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 0x38), 0x80, 0x28, 0);
        func_ov004_020b2444(0x90, 0x28, 3, 0, 0, 0, 0x28);
    }

    func_ov004_020afb20(data_ov006_0213ac30, 0x18, 0x30, -1, 0, 0x1000, self->mLamp1Angle);
    func_ov004_020afb20(data_ov006_0213ac3c, 0x40, 0x10, -1, 0, 0x1000, self->mLamp2Angle);

    return 1;
}

#pragma opt_strength_reduction on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN12dScMgSlot3_c8BehaviorEv, 0x0210bcb0, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_c8BehaviorEv
/* dScMgSlot3_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the two slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Steps the reel state machine through the pointer-to-member table at
 * data_ov006_02142bdc, ticks the frame counter at 0x503f, spins the two reel
 * offsets while state 1 is running, and flips the blend bit in data_0209d45c off a
 * coin toss so the machine's lights flicker.
 *
 * `Obj` stays a bare non-polymorphic stand-in rather than dScMgSlot3_c: a PMF on a
 * polymorphic class is a different representation, so the shape here is codegen and
 * not decoration. Its `idx` at 0x5000 is the header's mState, h5018/h501a the
 * header's mLamp1Angle/mLamp2Angle, and c503f its mFrameCounter.
 *
 * The three globals stay outside `extern "C"`, exactly as the pre-migration file
 * had them: a variable at global namespace scope is not mangled, so the spelling
 * that already linked is kept rather than churned. */
s32 dScMgSlot3_c::Behavior()
{
    Obj *self = (Obj *)this;
    int i;
    unsigned char t;

    unsigned char *pc;
    unsigned short *ph1;
    unsigned short *ph2;

    (self->*data_ov006_02142bdc[self->idx])();

    pc = (unsigned char *)((char *)self + 0x503f);
    *pc = *pc + 1;
    func_ov006_020c2144((char *)self + 0x4f38);

    for (i = 0; i < 3; i++) {
        if (self->idx == 1) {
            ph1 = (unsigned short *)((char *)self + 0x5018);
            ph2 = (unsigned short *)((char *)self + 0x501a);
            *ph1 = *ph1 - 0x200;
            *ph2 = *ph2 - 0x400;
            break;
        }
    }

    t = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 1;
    if (t)
        data_0209d45c |= 2;
    else
        data_0209d45c &= ~2;

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN12dScMgSlot3_c13InitResourcesEv, 0x0210bdb0, size 0x370 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot3_c13InitResourcesEv
/* dScMgSlot3_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgSlot3_c, this class's own
 * slot 0 (fBase_c::InitResources). The old file's `recovered name:
 * dScMgSlot3_c_InitResources` agreed.
 *
 * The `((struct Obj *)c)->v18(3)` is a self-dispatch through this class's own
 * vtable slot 18 -- left as a raw vtable-shim call, same shape the pre-migration
 * file used, just through `this` instead of a `void *arg0` parameter. */
s32 dScMgSlot3_c::InitResources()
{
    char *c = (char *)this;

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x1118;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1218;
    *(volatile u16 *)0x400000c = *(volatile u16 *)0x400000c & ~0x40;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1418;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;

    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~3;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~3;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    LoadCompressedFileAt(0x77, (void *)func_02054d88());

    {
        int h = LoadFile(0x78);
        _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(0x75, _ZN2G212GetBG1ScrPtrEv());
    LoadCompressedFileAt(0x7a, _ZN2G212GetBG2ScrPtrEv());

    {
        int h = LoadFile(0x79);
        func_02056314((void *)h, 0, 0x800);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(0xeb, (void *)0x6400000);

    data_0209d45c = 0x1f;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1218;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;

    LoadCompressedFileAt(0x77, _ZN3G2S13GetBG2CharPtrEv());
    LoadCompressedFileAt(0x76, _ZN3G2S12GetBG2ScrPtrEv());
    LoadCompressedFileAt(0xeb, (void *)0x6600000);

    {
        int h = LoadFile(0xec);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)h, 0xe0);
        _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        Deallocate((void *)h);
    }

    data_0209d454 = 0x14;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xc, 0xc, 4);

    func_ov006_020c2154(c + 0x4f38);
    func_ov006_020c1eb4(c + 0x4660);

    *(int *)(c + 0x5004) = *(int *)(c + 0xbc);

    ((struct Obj *)c)->v18(3);

    *(u8 *)(c + 0x5000 + 0x3b) = *(u8 *)(c + 0x5000 + 0x3c);
    *(int *)(c + 0x5000 + 0xc) = 0;

    for (int i = 0; i < 3; i++) {
        int rnd = RandomIntInternal(&data_0209e650);
        u32 divisor = *(u8 *)(c + 0x5000 + 0x3a);
        u32 val = ((u32)rnd >> 16) % divisor;
        ((u32 *)(c + 0x4fe4))[i] = val * 0x50000;
    }

    data_ov004_020bc88c = 0x80;
    data_ov004_020bc860 = 0xa0;
    data_ov004_020bc878 = 0x80;
    data_ov004_020bc890 = 0xa0;
    data_ov004_020bc8b8 = 0x80;
    func_020bc8b4 = 0x60;
    data_ov004_020bc888 = 0x80;
    data_ov004_020bc864 = ~0x1b;

    func_ov004_020b04d0(0x20);

    data_0208ee44 = 1;

    *(u16 *)(c + 0x5018) = 0;
    *(u16 *)(c + 0x501a) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- dScMgSlot3_c_classInit, 0x0210c120, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol dScMgSlot3_c_classInit
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  1 func_ov006_*                unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  3 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 *  2 _ZTV vptr store(s)          stands in for the ctor that would emit it.
 */
/* Reconstructed source-style name: SM64DS proves dScMgSlot3_c through RTTI,
 * allocation size, vtable identity, and the MG_SLOT3 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0210c120. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *dScMgSlot3_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x5044);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev((char *)p + 0x471c);
        /* EMITTING-TU spelling. This TU defines dScMgSlot3_c's key function, so
           mwccarm emits _ZTV12dScMgSlot3_c here, and mwccarm's _ZTV symbol
           addresses the vtable OBJECT -- two words of ABI preamble ahead of the
           slot array that config/arm9/overlays/ov006/symbols.txt names. &[2] is
           that same slot array; production isolation subtracts the preamble again
           when the definition is externalised. The dScMgSingle3DBase_c store
           below stays at +0: that vtable is only imported here, never emitted,
           so its symbol already IS the slot array. Same rule, opposite side, as
           src/game/actors/daTrs_c/d_a_trs_factories.cpp. */
        p[0] = (int)&_ZTV12dScMgSlot3_c[2];
        func_ov006_020c221c((char *)p + 0x4f38);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_0210c180, 0x0210c180, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c180
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c180(void *c_){
  char *c = (char *)c_;
  *(int*)c = 0x48;
  *(unsigned char*)(c+4) = 1;
  data_0209d454 |= 2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_0210c1a8, 0x0210c1a8, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c1a8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c1a8(void *o_)
{
    int *o = (int *)o_;
    if (*o <= 0)
        return;
    *o = *o - 1;
    if ((*o & 7) != 0)
        return;
    *((unsigned char *)(((int)o + 4))) ^= 1;
    if (*((unsigned char *)o + 4) != 0)
        data_0209d454 |= 2;
    else
        data_0209d454 &= ~2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_0210c208, 0x0210c208, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c208
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c208(char *p)
{
    *(int *)(p + 0x0) = 0;
    *(char *)(p + 0x4) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_0210c218, 0x0210c218, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c218
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c218(void *p_, s16 a, s16 b)
{
    unsigned char *p = (unsigned char *)p_;
    *(unsigned short *)(p + 0) = a;
    *(unsigned short *)(p + 2) = b;
    *(int *)(p + 4) = 0x48;
    *(unsigned char *)(p + 8) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_0210c234, 0x0210c234, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c234
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c234(unsigned char* o){
  if(*(unsigned char*)(o+8)==0) return;
  RenderOamMainScreen(&data_ov006_0213e6a8, *(short*)o, *(short*)(o+2), -1, -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_0210c278, 0x0210c278, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c278
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c278(void *o_)
{
    unsigned char *o = (unsigned char *)o_;
    if (*(int*)(o + 4) <= 0) return;
    int *p = (int*)(((int)o + 4));
    *p = *p - 1;
    if ((*(int*)(o + 4) & 7) == 0) {
        *((unsigned char*)(((int)o + 8))) ^= 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_0210c2b0, 0x0210c2b0, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c2b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2b0(char *p)
{
    *(int *)(p + 0x4) = 0;
    *(char *)(p + 0x8) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_0210c2c0, 0x0210c2c0, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c2c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2c0(void *o, int v) {
  int *p = (int *)o;
  p[8] = (p[7] * v) << 2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_0210c2d4, 0x0210c2d4, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c2d4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2d4(void *c_)
{
    char *c = (char *)c_;
    void *p;
    int v;
    unsigned int sid;

    if (*(int *)(c + 0x1c) >= 3) {
        return;
    }
    if (*(unsigned char *)(c + 0x10) != 0) {
        return;
    }
    p = data_ov004_020beb68;
    v = (p != 0) ? *(int *)((char *)p + 0xa8) : 0;
    if (v <= 0) {
        return;
    }
    *(int *)(((int)c + 0x1c)) =
        *(int *)(((int)c + 0x1c)) + 1;
    func_ov004_020b1b08((void *)1);
    func_ov001_020ab3f0(c);
    sid = 0x163;
    _ZN5Sound12PlayBank2_2DEj(sid);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_0210c354, 0x0210c354, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c354
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c354(void *p_)
{
    char *p = (char *)p_;
    *(int *)(p + 0x1c) = 0;
    *(int *)(p + 0x20) = 0;
    if (*(int *)(p + 0x14) >= 1)
        *(unsigned char *)(p + 0x11) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN12dScMgSlot1_c9betIcon_c6RenderEv, 0x0210c374, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c9betIcon_c6RenderEv
void dScMgSlot1_c::betIcon_c::Render()
{
    _ZN3OAM9RenderSubEP7OamAttriiii(
        data_ov006_0213e528[unk_010], unk_004, unk_006, -1, 1);
    int i;
    int sb = 0xb0;
    int z = 0;
    for (i = 0; i < unk_01c; i++) {
        func_ov004_020af948(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 8), 0xb0 + i*0x10, sb, (void*)z);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv, 0x0210c410, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
void dScMgSlot1_c::betIcon_c::Behavior()
{
    int val;
    if (TouchArea_Update(this, -1))
        func_ov006_0210c2d4(this);
    val = unk_020;
    if (val != 0) {
        if ((val & 3) == 0) {
            func_ov004_020b1b40(1);
            _ZN5Sound12PlayBank2_2DEj(0x149);
        }
        unk_020 -= 1;
    }
    dThIcon_c::Behavior();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_0210c478, 0x0210c478, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c478
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c478(char *c) {
    func_ov001_020ab5b0(c, 1, 0x90, 0xb0, 0x10, 8);
    *(int*)(c + 0x18) = 6;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN12dScMgSlot1_c19OnHitFromUnderneathEv, 0x0210c4b8, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c19OnHitFromUnderneathEv
/* dScMgSlot1_c::OnHitFromUnderneath -- slot 28, and a REAL OVERRIDE as of this
   commit: dScMgBase_c declares the slot now, so this stops being a new virtual
   that merely happened to land on index 28 and becomes the override the
   cartridge has.  It was the last virtual in this class that mwcc numbered for
   itself; include/dScMgSlot1_c.h's banner carries the arithmetic that retires
   with it.

   THE PARAMETER IS GONE, and unlike slot 27's it comes off on the base's
   evidence alone.  dScMgBase_c's body at ov004:0x020af04c writes r1
   (`add r1, r4, #0x4000`) before it ever reads it and touches no other
   argument register, so nothing in the family consumes a second argument.
   This body cannot corroborate that the way the slot-27 override did: it calls
   the base as its very first act, so a second argument would ride through r1
   untouched and leave no trace.  The `dActor_c &other` this file used to carry
   was copied from include/dActor_c.h's slot 28, a parallel hierarchy, and
   dActor_c.h has been wrong on every parameter list this campaign has
   measured.  What is NOT invisible is DISAGREEMENT: `(dActor_c &)` and `()`
   are different functions, so keeping it would have made this a new slot at 29
   and put _ZTV12dScMgSlot1_c back into DIFFERS -- with rombuild green the
   whole time.

   THE RETURN TYPE IS THE ONE THING HERE THAT IS NOT MEASURED, and slot 28 is
   the first in this campaign where that is true.  Neither body sets r0
   deliberately: the base falls out of a virtual call it has just compared
   against zero, and this one returns whatever SetSubBg1Offset left behind.  No
   caller loads vtable+0x70 anywhere in ov004 or ov006, so nothing consumes a
   result either.  `int` is include/dActor_c.h's, kept because it is what this
   file already carried and because dActor_c.h's RETURN types have held up
   where its parameter lists have not -- a hint, not a measurement.  `void`
   compiles to the same bytes.

   The forwarding call is written qualified, dScMgBase_c::OnHitFromUnderneath(),
   which suppresses the virtual dispatch and emits the same direct `bl` the ROM
   has -- straight to ov004:0x020af04c, not through ov006's veneer.  It was
   previously spelled as an extern "C" call to the base body's pre-rename name,
   which is the only spelling that was available while the base had not
   declared the member. */
int dScMgSlot1_c::OnHitFromUnderneath()
{
    dScMgBase_c::OnHitFromUnderneath();
    SetSubBg1Offset(0x100, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN12dScMgSlot1_c15OnHitByMegaCharEv, 0x0210c4dc, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c15OnHitByMegaCharEv
/* dScMgSlot1_c::OnHitByMegaChar -- slot 27, and a REAL OVERRIDE as of this
   commit: dScMgBase_c declares the slot now, so this stops being a new virtual
   that merely happened to land on index 27 and becomes the override the
   cartridge has.  include/dScMgSlot1_c.h's banner carries the arithmetic.

   THE PARAMETER IS GONE, and that is a measurement rather than a tidy-up.  This
   body opens `mov r4, r0` and then writes r0 and r1 with zero for
   SetSubBg1Offset; no second argument register is live on entry, and
   dScMgBase_c's own body reads none either.  The `Player &player` this file
   used to carry was copied from include/dActor_c.h's slot 27, a parallel
   hierarchy -- and dActor_c.h has now been wrong on every parameter list this
   campaign has measured.  Keeping it would have been worse than cosmetic:
   `(Player &)` and `()` are different functions, so this would have become a
   NEW slot at 28, pushed OnHitFromUnderneath to 29 and put _ZTV12dScMgSlot1_c
   back into DIFFERS -- with rombuild green the whole time.

   The return type is unchanged and still `void`.  dScMgBase_c's body measures
   it directly now (an early `popne {r4,lr}; bxne lr` with no r0 set), where
   before this file could only cite daObjPile_c::OnHitByMegaChar from the other
   branch.

   The forwarding call is written qualified, `dScMgBase_c::OnHitByMegaChar()`,
   which suppresses the virtual dispatch and emits the same direct `bl` the ROM
   has.  It was previously spelled as an `extern "C"` call to the base body's
   pre-rename name, which is the only spelling that was available while the
   base had not declared the member. */
void dScMgSlot1_c::OnHitByMegaChar()
{
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OnHitByMegaChar();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_0210c500, 0x0210c500, size 0x138 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c500
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_0210c500(void *self)
{
    struct T *p = (struct T *)self;
    int ok;
    int first;
    int i;

    p->count = 0;
    ok = 1;
    for (i = 0; i < 3; i++) {
        if (p->grid[i][p->idx[i]] == 5) {
            p->count++;
        } else {
            first = p->grid[i][p->idx[i]];
            break;
        }
    }
    if (p->count >= 3) {
        first = 5;
    } else if (i < 2) {
        for (i = i + 1; i < 3; i++) {
            u8 c = p->grid[i][p->idx[i]];
            if (c == 5) {
                p->count++;
            } else if (first != c) {
                ok = 0;
            }
        }
    }
    if (ok) {
        if (p->f708 < 2) {
            u8 *q = (u8 *)(int)((u8 *)p + 0x4708);
            *q = *q + 1;
        }
        p->f709 = first;
    }
    return ok;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov006_0210c638, 0x0210c638, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c638
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c638(void *thiz_)
{
    char *thiz = (char *)thiz_;
    *(signed char *)(thiz + 0x4706) = -1;
    *(unsigned char *)(thiz + 0x4707) = 1;
    *(unsigned char *)(thiz + 0x4708) = 0;
    *(int *)(thiz + 0xa8) = func_ov004_020ad8b8();
    *(int *)(thiz + 0xac) = *(int *)(thiz + 0xa8);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- _ZN12dScMgSlot1_c13OnYoshiTryEatEi, 0x0210c674, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c13OnYoshiTryEatEi
// recovered name: dScMgSlot1_c_OnYoshiTryEat_0210c674
/* dScMgSlot1_c::OnYoshiTryEat - recovered from vtable slot identity. Slot 18
   stays an unmigrated raw extern "C" helper (see include/dScMgSlot1_c.h),
   so this reaches its own fields via raw offsets on a char* rather than
   including the now-C++ class header -- 0x4706 is touched only here, not
   by any migrated method, same precedent dScMgPachinko2_c.h documents for
   its own offset 0xbc. */
void dScMgSlot1_c::OnYoshiTryEat(int i)
{
    char *c = (char *)this;

  if(i == 4){
    *(unsigned char*)(c + 0x4706) = *(unsigned char*)(c + 0x4709);
  } else if(i == 3){
    func_ov006_0210c638(c);
  }
  func_ov006_0210c354(c+0x4660);
  *(int*)(c + 0x46b4) = 0;
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- _ZN12dScMgSlot1_c6RenderEv, 0x0210c6c0, size 0x320 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c6RenderEv
/* dScMgSlot1_c::Render -- the recovered symbol at this address
   (_ZN3OAM7SECONDSE, i.e. "OAM::SECONDS") was a name-recovery-heuristic
   artifact, not a real name; this function genuinely is Render (see
   include/dScMgSlot1_c.h's file banner). mBetIcon.Render() is the ROM's
   virtual call through the typed nested betIcon_c member at 0x4660.
   `this+0xa8` is inherited from further up the hierarchy than dScMgBase_c,
   so it stays a raw offset on an unsigned char* cast. */
s32 dScMgSlot1_c::Render()
{
    unsigned char* t = (unsigned char*)this;
    int i, pos, x, j;
    unsigned char* cur = t;
    unsigned char b;
    for (i = 0; i < 3; i++) {
        pos = *(t + i + 0x46ff);
        x = data_ov006_0213e63c[i][1] - (*(int*)(t + (i << 2) + 0x46a4) >> 12);
        for (j = 0; j < 2; j++) {
            Hud_RenderSprite(data_ov006_0213e96c[*(cur + pos + 0x46c0)],
                                data_ov006_0213e63c[i][0], x, -1, 3);
            pos--;
            x += 0x40;
            if (pos < 0)
                pos += 0x15;
        }
        cur += 0x15;
    }
    mBetIcon.Render();
    func_ov004_020b2444(0x70, 0xb0, *(int*)(t + 0xa8), 0, 1, 1, 0x14);
    if (*(int*)(t + 0x46b4) >= 5 && (*(t + 0x470a) != 0 || (*(t + 0x470b) != 0 && *(t + 0x470b) < 3))) {
        b = *(t + 0x470b);
        if (b != 0 && b < 3) {
            i = 0;
            if ((int)b > 0) {
                pos = 0x18;
                do {
                    func_ov004_020af770(data_ov006_0213e96c[0xb], pos, 0x30, -1, 2, 0x1000, 0);
                    pos += 0x10;
                    i++;
                } while (i < *(t + 0x470b));
            }
            func_ov004_020af868(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 4), 0x50, 0x30, -1, 2, 0);
            func_ov004_020b2444(0x60, 0x30, *(t + 0x470b) * 2, 0, 2, 2, 0x14);
        }
        if (*(t + 0x470a) != 0) {
            i = 0;
            pos = 0x18;
            for (; i < 3; i++) {
                func_ov004_020af770(data_ov006_0213e96c[*(t + 0x4709) + 6], pos, 0x40, -1, 2, 0x1000, 0);
                pos += 0x10;
            }
            func_ov004_020af868(*(void **)(data_ov006_0213e5ec[GetGameLanguage()] + 4), 0x50, 0x40, -1, 2, 0);
            func_ov004_020b2444(0x60, 0x40, data_ov006_0213e4d8[*(t + 0x4709)], 0, 2, 2, 0x14);
        }
    }
    func_ov006_0210c234(t + 0x4684);
    func_ov006_0210c234(t + 0x4690);
    return 1;
}

#pragma opt_strength_reduction on

