// PORT_HOST_ABI. dScMgBase_c's two per-frame framework ticks that call VTABLE
// SLOT 19 THROUGH A SHADOW ARRAY, host-copied for the calling convention.
// Found by run mg9 lane BOO, on scene 387, and it is the FAMILY's defect
// rather than that class's.
//
// ---- WHAT IT WAS ----------------------------------------------------------
//
// src/func_ov004_020b6b40.c and src/func_ov004_020b6c9c.c are byte-identical
// apart from their symbol names, and each contains this:
//
//     int (*fn)(void *, int) = (int (*)(void *, int)) (*(void ***)g)[0x13];
//     if (fn(g, c->f18) == 0)
//         return;
//
// A PLAIN C FUNCTION POINTER, so MSVC compiles the call as __cdecl: it pushes
// two dwords and cleans eight bytes itself with `add esp, 8`. The thing it
// calls is hal/scene_mg.cpp's slot-19 host thunk
//
//     static int __fastcall mb_v19(void *, void *, int)
//
// which is __fastcall with ONE stack parameter, so it cleans that parameter
// itself and the compiled body ends `ret 4` (verified in the image at
// +0x0045611b). Four bytes are cleaned TWICE. Every call through either of
// these two bodies leaves esp four bytes high.
//
// THE THUNK IS RIGHT AND THE CALL SITE IS WRONG, which is worth stating
// because the obvious repair is the other way round. Run mg5 lane BASESET
// added that stack parameter on purpose: port/mg_fanout_costs.txt section 4
// records that both slot-18 and slot-19 thunks were declared with no stack
// parameter "while every ROM caller pushes one", and its census found 14 sites
// dispatching offset 0x4c with an argument count of ONE at every one. The ROM
// agrees here too -- 0x020b6cf4 is
//
//     ldr r2,[r0]        the vptr
//     ldr r1,[r4,#0x18]  the argument
//     ldr r2,[r2,#0x4c]  slot 19
//     blx r2
//
// with r0 the object and r1 the int, so slot 19 genuinely takes (this, int).
// The port has BOTH kinds of caller for it: hal-side dispatch sites that
// declare the pointer __fastcall and expect the callee to clean, and src-side
// shadow-array sites like these two that declare plain C and clean it
// themselves. One thunk signature cannot satisfy both, so the callee keeps the
// convention BASESET measured and the two src-side sites get a host copy that
// spells the pointer __fastcall. That is exactly the remedy
// port/mg_fanout_costs.txt section 13 records for
// unmatched/MgMemory2_ShadowSlot_020c06dc.cpp, whose defect is the same one
// seen from the other side: "MSVC's fourth word is Virtual10, which is
// __thiscall with one stack parameter and so returns with `ret 4`, eating four
// bytes the caller never pushed".
//
// ---- HOW IT PRESENTED, AND WHY NOTHING HAD SEEN IT BEFORE -----------------
//
// It is not a slow leak. The imbalance lands inside one function, so
// func_ov004_020b6c9c's own epilogue reads one stack slot too high:
//
//     pop esi   takes the saved ebp
//     pop ebp   takes the RETURN ADDRESS
//     ret       takes whatever the next word is
//
// On scene 387 that next word was 0x307fb4a4, an address inside the live scene
// object, and the process executed it: FAULT c0000005 at eip 307fb4a4 with
// access code 8, ebp = 0x0045b17b which faultmap resolves to
// port_mg_try_base_state+0x28b -- the return address, in the wrong register,
// which is the signature of this exact shape.
//
// NOTHING HAD SEEN IT BECAUSE NOTHING HAD RUN THE FRAMEWORK STATE MACHINE.
// port/mg_fanout_costs.txt section 13 reports zero framework calls on scene
// 363 and section 14 reports the same on 380 ("On scene 380 NOTHING fires"),
// and both close on the honest note that routing an address no run touches
// buys no evidence. dScMgTeresa_c is the first seated class whose boot
// actually enters _ZN10dMgState_c8SetStateEi: its scene-387 run reports 1 call into
// the setter, message index 18 asked for once, 1 state body and 10 per-frame
// tick entries routed. Message index 18 is func_ov004_020b6d6c, whose last act
// is `*(P2*)(c+8) = data_ov004_020bc924` -- and that pair reads
// {0x020b6c9c, 0} in overlay_0004.bin, which installs one of these two bodies
// as the per-frame tick. So the path is: the framework state machine runs at
// all -> message 18 -> tick 020b6c9c -> slot 19 -> four bytes -> a wild
// return. It took roughly 960 frames of scene 387 to arrive, deterministically.
//
// ---- WHAT DIVERGES FROM src/, EXACTLY -------------------------------------
//
// The function-pointer TYPE, in both bodies, and nothing else. `int (*)(void*,
// int)` becomes `int (__fastcall *)(void*, void*, int)` and the call gains the
// second (edx) argument the convention needs, which every host vtable thunk in
// this port already ignores. The two src TUs are commented out of
// port/slice_mg1.txt in the same commit rather than left in beside these
// definitions, because a src TU standing beside a real definition is a
// duplicate symbol and not a safety net -- port/mg_fanout_costs.txt section 16
// records lane L369 learning that with MgPachinko2_Traps.cpp.
//
// THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN. Nothing about the src is wrong on
// ARM: mwcc emits a plain `blx` and the ROM's own caller passes both values in
// registers, so the byte gate is green over it and will stay green. This is a
// host-ABI defect only, which is why it survived every gate the port has.

extern "C" {

struct P2 { int a; int b; };

extern void *data_ov004_020beb68;
extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern struct P2 data_02086b58;

void FreeGfxSlotsById(int arg);
int  func_ov004_020adbe0(void);
void func_ov004_020b6f14(void *c);
void func_ov004_020b7020(char *c);

void func_ov004_020b6b40(void *c);
void func_ov004_020b6c9c(void *c);

}  /* extern "C" */

struct ShadowObj { char pad[0x10]; struct P2 p10; int f18; int f1c; };

/* THE ONE CHANGED LINE, factored so the two bodies cannot drift apart. The
   src spells this `int (*fn)(void*,int) = (int(*)(void*,int))(*(void***)g)[0x13];`
   and calls fn(g, c->f18). Slot 0x13 is 19: 0x13 * 4 = 0x4c, which is the
   byte offset the ROM's own `ldr r2,[r2,#0x4c]` uses. */
typedef int (__fastcall *MgSlot19)(void *thiz, void *edx, int arg);

/* the shadow_slot19 helper RETIRED (run link100, lane SEAT6, batch B6).
   THE ONE CHANGED LINE this file exists for -- the shadow-array pointer
   spelled __fastcall so mb_v19's one stack parameter is cleaned once
   instead of twice -- is one line in each of the two byte-identical
   sources, and hostgen can make it there.
   The matched TU src/func_ov004_020b6b40.c and src/func_ov004_020b6c9c.c is seated in its place: port/tools/hostgen.py's VIRTUAL_CALL table, two rows carrying the same
   patch text because the two src TUs are byte-identical apart from
   their names.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */

/* src/func_ov004_020b6b40.c and src/func_ov004_020b6c9c.c, verbatim apart from
   the call above. The ROM has two copies of this body -- 0x020b6b40 and
   0x020b6c9c disassemble instruction for instruction the same and the two src
   TUs are byte-identical apart from their symbol names -- so they are written
   once here and instantiated twice. */
/* the shared shadow_tick body RETIRED (run link100, lane SEAT6, batch B6).
   Written once here because the ROM has the body twice; both instances
   come from their own matched sources now.
   The matched TU src/func_ov004_020b6b40.c and src/func_ov004_020b6c9c.c is seated in its place: see the note above.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */

// PORT_HOST_ABI: vtable slot 19 shadow-array call respelled __fastcall so MSVC does not double-clean the one stack parameter the host thunk already cleans
/* func_ov004_020b6b40 RETIRED (run link100, lane SEAT6, batch B6).
   Instantiation of the shared body above.
   The matched TU src/func_ov004_020b6b40.c is seated in its place: see the note above.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
// PORT_HOST_ABI: vtable slot 19 shadow-array call respelled __fastcall so MSVC does not double-clean the one stack parameter the host thunk already cleans
/* func_ov004_020b6c9c RETIRED (run link100, lane SEAT6, batch B6).
   Instantiation of the shared body above.
   The matched TU src/func_ov004_020b6c9c.c is seated in its place: see the note above.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
