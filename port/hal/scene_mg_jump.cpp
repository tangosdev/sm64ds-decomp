// dScMgJump_c -- actor id 0x174, scene 372, the "Bounce and Pounce" minigame.
// Run mg11, lane BNP. The per-class seat; port/slice_bnp.txt is the derivation
// and unmatched/MgBounceAndPounce_StateDispatch.cpp is the dispatch half.
//
// ---- 0. WHAT THIS CLASS IS, AND HOW ITS NAME AND ITS TITLE WERE DERIVED ----
//
// CLASS NAME, from the ROM's own RTTI. The word BEFORE the vtable
// (0x0213cbe0) points at 0x0213cb98; that type_info record is
// {0x0209a764, 0x0213cba4, 0x0213c5c8} and the name pointer 0x0213cba4 reads
// "11dScMgJump_c". Read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0, the shipped image, never a dsd export.
//
// PLAYER TITLE, from the ROM's own message bank and NOT from the spawn symbol.
// hal/scene_mg_sound.cpp's ov005 table has EXACTLY ONE row for id 0x0174 --
// row 4, param 0x00040400 -- so (param >> 8) & 0xff = 4 is the save record and
// (param >> 16) & 0xff = 4 is the name-text index. data_ov004_020bc070[4] = 552
// (inside lane S364's arithmetic-safe range 0..23, and read out of ov004 rather
// than computed anyway), and BMG message 552 of data/message/msg_data_eng.bin
// decodes to
//
//     Bounce and Pounce
//     Mario's in free fall, but he can bounce back if you touch him. Stomp the
//     Fly Guys, but make sure he doesn't fall off the Touch Screen.
//
// The sibling id 0x175 is row 20, text 28, message 576 = "Bounce and Trounce",
// a DIFFERENT record and a different title, so the two are not two names for
// one game. port/slice_s75.txt line 108 already carried the pairing
// dScMgJump_c/dScMgJump2_c -> Bounce and Pounce / and Trounce, which this lane
// re-derived from the ROM rather than inherited.
//
// ---- 1. THE FAMILY, AND THE THING THE CONFIG'S NAMES GET WRONG -------------
//
// dScMgJump_c does not derive from dScMgBase_c directly. Its type_info's base
// pointer is 0x0213c5c8, whose name reads "14dScMgD3DBase_c", and FOUR classes
// point at that record: 0x0213cba0 (this class), 0x0213ccac (0x175
// dScMgJump2_c), 0x0213fad8 (0x180) and 0x0213fc0c (0x181). dScMgD3DBase_c has
// its OWN 36-slot vtable at 0x0213c62c, the word before it being 0x0213c628 =
// 0x0213c5c8, the same type_info.
//
// SO ONLY EIGHT OF THIS CLASS'S TWENTY-THREE "OVERRIDES" ARE ITS OWN. Comparing
// 0x0213c62c against 0x0213cbe4 word by word out of the ROM, exactly slots 0,
// 3, 6, 9, 16, 17, 18 and 19 differ; the other twenty-eight words are
// identical, and fifteen of those twenty-eight are ov006 bodies that
// port/mg_fanout_costs.txt section 3 counts as this class's overrides because
// it compares against dScMgBase_c. They are dScMgD3DBase_c's, and ids 0x175,
// 0x180 and 0x181 hold most of them too.
//
// AND THE CONFIG NAMES THEM AFTER THIS CLASS, WHICH IS A TRAP FOR THE NEXT
// LANE. config/arm9/overlays/ov006/symbols.txt gives the base's code block
// (0x020e6c28..0x020e740c) names like _ZN17MgBounceAndPounce19BeforeInitResou
// rcesEv and _ZN17MgBounceAndPounceD1Ev, and it gives the name
// _ZTV17MgBounceAndPounce to address 0x0213c62c -- THE BASE'S VTABLE, not this
// class's. This class's own table is only ever spelled data_ov006_0213cbe4.
// A seat that wired "the class's vtable" by symbol NAME would fill
// dScMgD3DBase_c's table and leave dScMgJump_c's thirty-six words raw. This
// file fills BOTH, deliberately, and section 3 says why both are needed.
//
// ---- 2. NEITHER SIBLING HAS AN OVERLAY CONSTRUCTOR -------------------------
//
// Run mg11 seats 0x174 and 0x175 in two concurrent lanes, and the standing
// warning is that sibling classes share sinit regions and that address
// adjacency over-attributes (the MgMemoryMatch / MgMemoryMaster precedent, and
// lane LKY's near miss). Measured rather than assumed, on both spans:
//
//   NOT ONE relocation in config/arm9/overlays/ov006/relocs.txt whose source
//   lies in ov006's .init range (0x0212f4c4..0x0213356c) lands anywhere in
//   0x0213cb48..0x0213cc74, which is the whole of dScMgJump_c's .data, nor
//   anywhere in 0x0213cc74..0x0213cd8c, which is the whole of dScMgJump2_c's.
//
// So THIS LANE CLAIMS NO SINIT RANGE AT ALL and leaves none for lane BNT
// either: there is no constructor of this family's among the thirty-five, the
// state pairs are statically initialised in the ROM image, and the shared-sinit
// attribution trap cannot arise here. The class boundary in .data is exact:
// this class's span ends at 0x0213cc74, whose {0x020ef47c, 0} pair points into
// 0x175's code block and is 0x175's first word.
//
// ---- 3. THE TWO TABLES, AND WHY THE BASE ONE IS NOT CEREMONY ---------------
//
// src/MgBounceAndPounce_Spawn.cpp -- the ROM factory at 0x020eeafc, verified
// instruction for instruction -- writes the BASE table into the object's first
// word (0x020eeb20), builds the sub-objects, and only then writes this class's
// own table (0x020eeb60). Both destructors run the same sequence in reverse:
// src/func_ov006_020edec0.cpp (slot 16) and src/func_ov006_020edf54.cpp (slot
// 17) each write data_ov006_0213cbe4, destroy the arrays, then write
// _ZTV17MgBounceAndPounce (0x0213c62c) before calling the base destructor. So
// the object genuinely dispatches through the BASE table during construction
// and teardown, and a base table left holding raw DS words is a wild call in
// exactly those windows.
//
// THE ORDERING RULE APPLIES TO THE BASE TABLE THE WAY dScMgSingle3DBase_c's
// APPLIES TO THE FLOWER ROW. mg_apply keys on a DS address, so the fill that
// runs FIRST claims the base table and later ones find nothing left to write.
// This seat is the FIRST of the four dScMgD3DBase_c classes the port has
// hosted, so it claims 0x0213c62c and the counts below say so out loud; lanes
// seating 0x175, 0x180 and 0x181 will find it already claimed, which is
// correct and is why their own base-table claim counts will read zero.
//
// ---- 4. THE ONE FLOOR -- RETIRED. WHAT IT WAS AND WHAT IT COST -------------
//
// RUN mg12, LANE IGN RETIRED THIS FLOOR. src/func_ov006_020ee994.c now exists,
// the seat routes the real body at slot 18 (bnp_v18 below), and everything
// this section describes as "does not happen" NOW HAPPENS. The section is kept
// as written, in the past tense where it matters, because it is the derivation
// the decomp was written from and because the four hidden callees it names are
// the four slice lines the retirement cost. The TU is an honest NONMATCHING --
// 0x164 against the ROM's 0x168, ONE codegen divergence (three loop-invariant
// constants the ROM rematerialises per iteration and every mwccarm hoists),
// logic verified instruction for instruction. Its own banner carries the
// decomposition and the full list of levers tried.
//
// THE MEASURED CONSEQUENCE, both ends, same binary, scene 372 unattended:
// with the trap, the state word at self+0x5004 stayed 0x00000000 for the whole
// run and slot 6 asked with code 0 every tick. With the ignition live it
// installs 0x020ee5b8 (the opening countdown) on the first tick and the chain
// walks. The witness at the bottom of this file prints both halves.
//
// WHAT WAS TRUE WHILE IT WAS A FLOOR:
//
// Vtable slot 18 is func_ov006_020ee994: a config symbol with a size
// (0x168 bytes), NO delink block -- the block before it ends at 0x020ee994 and
// the next starts at 0x020eeafc, which is the body's own size to the byte --
// and NO src file in either extension anywhere in the tree. It is the ONLY gap
// in this class's code: a delink walk over 0x020edec0..0x020eebe8 finds
// EIGHTEEN delink blocks and exactly one hole, this one -- while ov006's symbol
// table names NINETEEN functions in the same span, the difference being the
// floor itself, which has a symbol and no block. It is trapped below, and the
// trap is honest about what it costs, because this floor is not an ordinary
// one:
//
//   THE FLOOR IS THE ONLY BODY THAT STARTS THE STATE MACHINE. Slot 0
//   (InitResources, src/func_ov006_020ee690.cpp) ends with a dispatch through
//   the object's own vtable at offset 0x48 -- slot 18 -- with the argument -1,
//   and the ROM's slot-18 body tail-calls func_ov006_020ee658, whose only job
//   is to copy the pair at 0x0213cb54 into self+0x5004. Nothing else in ov006
//   writes that field before slot 6 first reads it. With the trap in place the
//   field stays zero, and the address switch reports zero routed dispatches
//   with every ask carrying code 0x00000000.
//
// WHAT ELSE THE FLOOR DOES, disassembled rather than left mysterious, so the
// decomp that retires it starts at the seat:
//
//   r1 == 0     ++self->+0xbc (the round counter), clamped at 0x270e
//   r1 == 0x12  self->+0xbc = 0, then the global at data_ov004_020beb68 gets
//               its +0xb4 zeroed and func_ov004_020adb1c(self->+0xb4) runs
//   otherwise   that global's +0xb4 = self->+0xbc
//   then        func_ov006_020c72b4() if +0xbc != 0 else func_ov006_020c72c8()
//   then        if +0xbc >= 0xf, roll func_02043b990 -> ((rand & 0x7fffffff)
//               >> 0x13) * 10 >> 12, a 0..9 draw, REROLLED while it equals the
//               cached last draw in data_ov006_0213cb48 (initialised 0xffffffff
//               in the ROM image, which is why that word is a scratch int and
//               not the adjustment half of a pair)
//   then        func_ov006_020c719c(+0xbc, draw)
//   then        data_ov006_02140328 = 3 / 4 / 5 / 6 for +0xbc < 0xf / < 0x14 /
//               < 0x19 / else -- the difficulty ladder
//   then        func_ov006_020c44b4(+0xbc, draw)
//   tail        func_ov006_020ee658(self), the state-0 installer
//   returns     self + 0x5000 (single exit; a nonzero pointer, so a caller
//               testing it as a truth value sees true -- the trap returns 1,
//               which is the same answer to that test and is not the same word)
//
// SO WITH THE TRAP IN PLACE: the class boots, InitResources loads its files and
// lays out the two camera records and the nine sub-objects, Render draws every
// frame, and NOTHING MOVES -- no round is dealt, the difficulty global is never
// written, the Fly Guys are never placed by func_ov006_020c719c /
// func_ov006_020c44b4, and the state machine never leaves the null pair. That
// is a decomp gap, not a port one, and it is lane S364's slot-6 shape exactly:
// that floor was retired later by one transcription and this one can be too.
//
// THE RUN LAW SAYS A TRAP-SHAPED FLOOR HIDES ITS CALLEES FROM STATIC CLOSURE,
// AND HERE IT HIDES FOUR: func_ov006_020c72b4, _020c72c8, _020c719c and
// _020c44b4 are reached ONLY from this body in this class's closure, so they
// are not in port/slice_bnp.txt and retiring the floor costs four slice lines
// plus the transcription. All four have src files today.
//
// ---- 5. THE DIAGNOSTIC THAT STOOD IN FOR IT, NOW REMOVED -------------------
//
// SM64DS_BNP_START_STATE=1 used to make the trap, after counting, ALSO call
// func_ov006_020ee658 -- the ROM's own state-0 installer -- and nothing else
// the floor would have done. It was a DIAGNOSTIC and not a repair: with it on
// the state machine started at the countdown and walked, so the address switch
// could be shown to route, while the round counter, the difficulty ladder, the
// random draw and the two placement calls all stayed unrun.
//
// IT IS GONE, and removing it was not tidying. The real body ends with that
// same call, so leaving the variable wired would have installed the state pair
// a SECOND time after the ignition already installed it -- a diagnostic that
// used to substitute for the floor becomes a corruption of it the moment the
// floor is retired. Any run recipe still passing SM64DS_BNP_START_STATE=1 gets
// no effect and no warning from the environment; it is named here so a reader
// of an older banked log knows what that line meant.
//
// EVERY MEASUREMENT BANKED UNDER THAT VARIABLE REMAINS VALID AS WHAT IT SAID
// IT WAS -- evidence about the dispatch half only. Run mg11's 1153/1153 routed
// and its five-in-order states were taken that way, and lane IGN's proofs
// reproduce them with no variable set at all.

// ---- 6. THE LINKER ROWS THIS SEAT OWES, AND WHY EACH ONE EXISTS -----------
//
// Every row below was produced by port/tools/closure.py against this slice and
// then checked one at a time; none is a guess and none is a rename of a symbol
// that already resolves.
//
// TWENTY-SIX DATA ROWS, ALL THE SAME SHAPE. The ov006 mount (ov006_syms.c) is
// a .c compiled as C, so it defines every overlay datum under its C name
// (_data_ov006_xxxxxxxx). Sixteen of this slice's TUs are .cpp files that
// declare those data OUTSIDE extern "C", so MSVC asks for the C++ mangling and
// the link would fail on a symbol whose bytes are already in the image. Each
// row binds one C++ spelling to the mount's own definition. Two spellings of
// data_ov006_0214042c appear because two TUs declare the same address at two
// types (`int` and `void *`); both bind to the one definition, which is the
// mount's, so the bytes are shared and not duplicated. This is the treatment
// hal/scene_mg_amida.cpp uses for the same problem.
//
// FOUR FUNCTION ROWS, EACH FOR ITS OWN REASON:
//
//   _Scene_AfterRender -- src/_ZN17MgBounceAndPounce11AfterRenderEj.cpp calls
//   it by a name the decomp's naming pass invented; no config symbol has it.
//   The ROM says what it is: 0x020e700c's tail jump reads its target out of the
//   pool word at 0x020e703c, which is 0x0202e398, and config/arm9/symbols.txt
//   line 1105 names 0x0202e398 _ZN5Scene11AfterRenderEj.
//
//   _func_020beb74 -- src/func_ov006_020e7124.c declares `extern int
//   func_020beb74[]` and INDEXES it, so it is data wearing a func_ prefix. The
//   real symbol is data_ov004_020beb74 (kind:bss in ov004's symbols.txt), which
//   the ov004 mount already defines.
//
//   Animation::LoadFile -- the TU declares the static at C++ linkage; the
//   matched body is in the image under its flat Itanium C name.
//
//   ModelAnim::SetAnim -- src/func_ov006_020c6e4c.cpp and three siblings
//   declare it returning int where the image's definition returns void, so the
//   two manglings differ in exactly one character (QAEH vs QAEX). Binding the
//   int spelling onto the void one is the port's standing treatment for that
//   pair and the map already carries the H/I/X spread for Player::SetAnim.
//
// ALL FOUR TARGETS WERE CONFIRMED PRESENT IN THE BASELINE MAP BEFORE THE ROW
// WAS WRITTEN, which is what stops a row from silently binding a name to
// nothing.
#pragma comment(linker, "/alternatename:?data_0209f5c0@@3PAUGObj@@A=_data_0209f5c0")
#pragma comment(linker, "/alternatename:?data_0209f5f8@@3EA=_data_0209f5f8")
#pragma comment(linker, "/alternatename:?data_ov006_0212ddd0@@3DA=_data_ov006_0212ddd0")
#pragma comment(linker, "/alternatename:?data_ov006_02134d1c@@3PAUOamAttr@@A=_data_ov006_02134d1c")
#pragma comment(linker, "/alternatename:?data_ov006_0213afd8@@3HA=_data_ov006_0213afd8")
#pragma comment(linker, "/alternatename:?data_ov006_0213b088@@3PAHA=_data_ov006_0213b088")
#pragma comment(linker, "/alternatename:?data_ov006_0213b090@@3PAHA=_data_ov006_0213b090")
#pragma comment(linker, "/alternatename:?data_ov006_0213b0cc@@3HA=_data_ov006_0213b0cc")
#pragma comment(linker, "/alternatename:?data_ov006_02140330@@3USharedFilePtr@@A=_data_ov006_02140330")
#pragma comment(linker, "/alternatename:?data_ov006_02140338@@3USharedFilePtr@@A=_data_ov006_02140338")
#pragma comment(linker, "/alternatename:?data_ov006_02140400@@3EA=_data_ov006_02140400")
#pragma comment(linker, "/alternatename:?data_ov006_02140404@@3GA=_data_ov006_02140404")
#pragma comment(linker, "/alternatename:?data_ov006_02140408@@3HA=_data_ov006_02140408")
#pragma comment(linker, "/alternatename:?data_ov006_0214040c@@3HA=_data_ov006_0214040c")
#pragma comment(linker, "/alternatename:?data_ov006_02140424@@3HA=_data_ov006_02140424")
#pragma comment(linker, "/alternatename:?data_ov006_0214042c@@3HA=_data_ov006_0214042c")
#pragma comment(linker, "/alternatename:?data_ov006_0214042c@@3PAXA=_data_ov006_0214042c")
#pragma comment(linker, "/alternatename:?data_ov006_02140430@@3HA=_data_ov006_02140430")
#pragma comment(linker, "/alternatename:?data_ov006_02140438@@3USharedFilePtr@@A=_data_ov006_02140438")
#pragma comment(linker, "/alternatename:?data_ov006_02140440@@3USharedFilePtr@@A=_data_ov006_02140440")
#pragma comment(linker, "/alternatename:?data_ov006_02140448@@3USharedFilePtr@@A=_data_ov006_02140448")
#pragma comment(linker, "/alternatename:?data_ov006_02140450@@3USharedFilePtr@@A=_data_ov006_02140450")
#pragma comment(linker, "/alternatename:?data_ov006_02140458@@3USharedFilePtr@@A=_data_ov006_02140458")
#pragma comment(linker, "/alternatename:?data_ov006_02140460@@3USharedFilePtr@@A=_data_ov006_02140460")
#pragma comment(linker, "/alternatename:?data_ov006_02140468@@3USharedFilePtr@@A=_data_ov006_02140468")
#pragma comment(linker, "/alternatename:?data_ov006_02141a44@@3PAXA=_data_ov006_02141a44")
#pragma comment(linker, "/alternatename:?data_ov006_02141a48@@3PAXA=_data_ov006_02141a48")
#pragma comment(linker, "/alternatename:_Scene_AfterRender=__ZN5Scene11AfterRenderEj")
#pragma comment(linker, "/alternatename:_func_020beb74=_data_ov004_020beb74")
/* _func_ov006_020e6df0 -- the SAME name-spelling shape as _func_020beb74, in
   the other direction. src/func_ov006_020c7c68.c calls the body at 0x020e6df0
   by its address-shaped name, but config/arm9/overlays/ov006/symbols.txt gives
   that address the RECOVERED name Sound_PlayBank1Panned, and
   src/Sound_PlayBank1Panned.cpp defines it under that name inside extern "C".
   One body, two spellings, and the caller has the one the config retired. */
#pragma comment(linker, "/alternatename:_func_ov006_020e6df0=_Sound_PlayBank1Panned")
#pragma comment(linker, "/alternatename:?LoadFile@Animation@@SAPAUBCA_File@@AAUSharedFilePtr@@@Z=__ZN9Animation8LoadFileER13SharedFilePtr")
/* The int-spelled ModelAnim::SetAnim alias that used to sit here MOVED to the
   ROM-faithful int chain at the mg11 merge (unmatched/MgJump2_Faces.cpp): the
   void mapping was safe only while every caller discarded the result. */

#include "hal/screen_gap.h"

/* The class the four real C++ methods are members of. It is the SAME generated
   header the four src TUs include, so the declarations the seat calls through
   and the definitions the linker finds are one text. */
#include "MgBounceAndPounce.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* the framework's own dispatch census, from unmatched/MgBase_StateDispatch.cpp */
void port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the SHARED object-state dispatch half,
   unmatched/MgD3DBase_ObjStateDispatch.cpp -- the second and third
   pointer-to-member walls in this closure, neither of them this class's */
void     port_mg_objstate_counts(unsigned *calls, unsigned *hits,
                                 unsigned *nullpmf);
unsigned port_mg_objstate_count(void);
unsigned port_mg_objstate_addr(unsigned i);
unsigned port_mg_objstate_hit(unsigned i);

/* this class's dispatch half, unmatched/MgBounceAndPounce_StateDispatch.cpp */
void     port_mg_jump_counts(unsigned *calls, unsigned *hits, unsigned *nullpmf);
unsigned port_mg_jump_state_count(void);
unsigned port_mg_jump_state_addr(unsigned i);
unsigned port_mg_jump_state_hit(unsigned i);
unsigned port_mg_jump_state_last(void);

/* the two mounted tables this seat fills. Both are inside their overlay's
   mount, so the fill replaces the mount's own words rather than building a
   fresh host array -- the ov007, curling and wiggler treatment, for the same
   reason: a second host array of the same name is a duplicate symbol, and
   leaving a mounted table alone leaves live wild DS pointers in a table the
   factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,      36 slots */
extern unsigned char _ZTV17MgBounceAndPounce[]; /* dScMgD3DBase_c, 36 slots,
                                                   address 0x0213c62c -- see
                                                   section 1 for the name */
extern unsigned char data_ov006_0213cbe4[];   /* dScMgJump_c,      36 slots */

/* ---- this class's own eight -------------------------------------------- */
int   func_ov006_020ee690(void *self);          /* slot 0  InitResources */
int   func_ov006_020edffc(void);                /* slot 3  CleanupResources */
int   func_ov006_020ee27c(void *self);          /* slot 6  Behavior, HOST COPY */
int   func_ov006_020ee034(void *self);          /* slot 9  Render */
void *func_ov006_020edec0(void *self);          /* slot 16 D2 */
void *func_ov006_020edf54(void *self);          /* slot 17 D0 */
/* slot 18 is func_ov006_020ee994, THE IGNITION. It was this class's one floor
   and run mg12's lane IGN retired it: src/func_ov006_020ee994.c is an honest
   NONMATCHING TU (one codegen divergence, logic verified instruction for
   instruction) and this seat now routes the real body. The ROM sets no return
   value of its own -- it falls out of the tail call to func_ov006_020ee658 --
   and slot 0 declares m48 void, so the thunk below reports 1 the way every
   other void face in this file does. */
void  func_ov006_020ee994(void *self, int sel);
int   func_ov006_020ee8dc(void *self, int sel); /* slot 19, READS arg2 */

/* ---- dScMgD3DBase_c's fifteen, shared with ids 0x175, 0x180 and 0x181 ----
 *
 * FOUR OF THEM ARE REAL C++ METHODS AND CANNOT BE REACHED BY THE ITANIUM NAME,
 * which cost this lane one link. src/_ZN17MgBounceAndPounce19BeforeInitResourc
 * esEv.cpp, _14BeforeBehaviorEv.cpp, _21AfterCleanupResourcesEj.cpp and
 * _11AfterRenderEj.cpp define their bodies as `int MgBounceAndPounce::Method()`
 * rather than inside extern "C", so MSVC emits only its own mangling
 * (?BeforeInitResources@MgBounceAndPounce@@QAEHXZ) and the flat _ZN name the
 * other eleven use resolves to nothing. This is the MSVC-NAME SHADOW shape
 * port/tools/linkage.py reports as its second join. The fix is to call them
 * THROUGH THE CLASS, out of the generated header the src TUs themselves
 * include, rather than to invent four /alternatename rows onto manglings the
 * seat would then have to keep in sync by hand.
 *
 * The other ELEVEN really are extern "C" with the Itanium name as the C
 * identifier and are declared as such below.
 */
/* slot 2, the SHARED SLOT-2 REPAIR. NOT the src symbol: the matched TU drops
 * the framework's second argument at its own call site and that argument
 * decides whether the actor is destroyed on frame 0. See
 * unmatched/MgD3DBase_Slot2.cpp, which carries the measurement. */
int   port_mg_d3dbase_after_init(void *c, unsigned f);
int   port_mg_d3dbase_before_render(void *c);   /* slot 10, SHARED host copy */
void  port_mg_d3dbase_slot27(void *c);          /* slot 27, SHARED host copy */
void  port_mg_d3dbase_slot28(void *c);          /* slot 28, SHARED host copy */
int   func_ov006_020e6e78(void *self);          /* slot 24 */
int   func_ov006_020e6e54(void *self);          /* slot 25 */
int   func_ov006_020e6e4c(void);                /* slot 26, `mov r0,#2; bx lr` */
void  func_ov006_020e6d24(void *self);          /* slot 29 */
void  func_ov006_020e6cac(void *self);          /* slot 30 */
void  func_ov006_020e72c0(void *self);          /* slot 31 */
void  func_ov006_020e7124(void *self);          /* slot 33 */

/* ---- dScMgD3DBase_c's own two destructors, in ITS table's slots 16/17 ---- */
int   _ZN17MgBounceAndPounceD1Ev(void *self);
int   _ZN17MgBounceAndPounceD0Ev(void *self);

/* globals the SM64DS_BNP_TRACE render probe reads */
extern int   data_ov006_02140328;
extern int   data_ov006_02140324;
extern int   data_ov006_02140428;
extern int   data_ov006_02140304;
extern int   data_ov006_02140418[];
extern char *data_ov006_02140420[];

/* The ROM's own state-0 installer. This seat no longer calls it: the section-5
   diagnostic that did is gone, and the ignition at slot 18 reaches it the way
   the ROM does. Kept declared because the witness prose names it. */
void  func_ov006_020ee658(void *self);

/* the factory */
void *MgBounceAndPounce_Spawn(void);

/* this seat's own exports */
void  port_scene_fill_jump(void);
void *port_mg_jump_spawn(void);
void  port_scene_jump_hits(void);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this port
// carries, for the reason hal/scene_mg.cpp gives: an object that EXISTS and an
// object that RUNS look identical from outside. These count only the slots THIS
// file's faces own -- this class's eight plus dScMgD3DBase_c's seventeen -- and
// the framework slots dScMgBase_c keeps are counted by scene_mg.cpp's own array.
static unsigned g_bnp_hits[36];
static unsigned g_bnp_base_hits[36];   /* dispatches that arrived on the BASE
                                          table's own two destructor slots */

#define BNP(n)  (++g_bnp_hits[(n)])

/* THE STATE WITNESS. Sampled at the top of every Behavior tick, before the ROM
   body reads the field, so it records what the class was about to dispatch
   rather than what it left behind. This class has no state INDEX; what moves is
   the code word of the pair at self+0x5004. */
static unsigned g_bnp_state_first;
static unsigned g_bnp_state_last;
static unsigned g_bnp_state_changes;
static unsigned g_bnp_state_seen[8];
static unsigned g_bnp_state_nseen;

static void bnp_sample_state(const char *s)
{
    const unsigned code = *(const unsigned *)(s + 0x5004);
    if (g_bnp_state_last == 0 && g_bnp_state_first == 0)
        g_bnp_state_first = code;
    if (code != g_bnp_state_last) {
        ++g_bnp_state_changes;
        g_bnp_state_last = code;
        unsigned k = 0;
        for (; k < g_bnp_state_nseen; ++k)
            if (g_bnp_state_seen[k] == code)
                break;
        if (k == g_bnp_state_nseen &&
            g_bnp_state_nseen < sizeof g_bnp_state_seen / sizeof g_bnp_state_seen[0])
            g_bnp_state_seen[g_bnp_state_nseen++] = code;
    }
}

/* SM64DS_BNP_TRACE=1: per-frame trace of the state pair, the round counter and
   the two camera records. OFF unless set, writes nothing otherwise, and it
   exists because this class's "state" is an address rather than an index and a
   dispatch count alone cannot say whether the machine moved. */
static void bnp_trace(const char *s, int tick)
{
    static unsigned last;
    const unsigned code = *(const unsigned *)(s + 0x5004);
    if (code != last || (tick % 120) == 0) {
        last = code;
        std::fprintf(stderr, "  [bnp] t%-5d state=0x%08x adj=%d  round=+0x%x=%d"
                     "  timer=%d  y=%d  vy=%d\n",
                     tick, code, *(const int *)(s + 0x5008),
                     0xbc, *(const int *)(s + 0xbc),
                     (int)*(const short *)(s + 0x5014),
                     *(const int *)(s + 0x500c), *(const int *)(s + 0x5010));
        std::fflush(stderr);
    }
}

static char *g_bnp_self;

// ---- this class's own eight ------------------------------------------------

static int __fastcall bnp_init(void *s, void *)
{
    BNP(0);
    g_bnp_self = (char *)s;
    const int r = func_ov006_020ee690(s);
    hal_gapless_minigames_latch();
    return r;
}

/* SLOT 3 GENUINELY TAKES NO RECEIVER. The ROM body at 0x020edffc never reads
   r0: it releases the SharedFilePtr in data_ov006_02142184, nulls it and calls
   func_ov004_020ad90c. src/func_ov006_020edffc.c declares `int f(void)` and
   that is right, not a dropped receiver. */
static int __fastcall bnp_clean(void *, void *)
{ BNP(3); return func_ov006_020edffc(); }

static int __fastcall bnp_beh(void *s, void *)
{
    BNP(6);
    static int tick;
    static int trace = -2;
    if (trace == -2)
        trace = std::getenv("SM64DS_BNP_TRACE") ? 1 : 0;
    ++tick;
    bnp_sample_state((const char *)s);
    if (trace > 0)
        bnp_trace((const char *)s, tick);
    return func_ov006_020ee27c(s);
}

/* SM64DS_BNP_TRACE=1 also prints the ELEMENT VTABLE this class's Render
   dispatches through, ONCE, before the first frame's dispatch. Slot 9 ends with
   `((void(**)(void*,void*))(*(int*)(c+0x501c)))[5](c+0x501c, &t)` -- the Model
   the factory constructs at that offset -- and run mg9 lane S381's headline is
   that a factory-built sub-object carries its OWN table in word 0. A null in
   that table's slot 5 is a jump to address zero from inside a body the seat
   never sees, so the word is printed rather than assumed. */
static int __fastcall bnp_render(void *s, void *)
{
    BNP(9);
    static int shown;
    if (!shown && std::getenv("SM64DS_BNP_TRACE")) {
        shown = 1;
        void **vt = *(void ***)((char *)s + 0x501c);
        std::fprintf(stderr, "  [bnp] Render element object at %p, its vtable "
                     "%p, slot5 %p, slot4 %p\n",
                     (void *)((char *)s + 0x501c), (void *)vt,
                     vt ? vt[5] : 0, vt ? vt[4] : 0);
        std::fprintf(stderr, "  [bnp] Render inputs: +0x4664=%u  +0x5024=%p  "
                     "*(+0x5028)=%p  loop counts 02140328=%d 02140418=%d  "
                     "list base 02140420=%p 02140324=%d\n",
                     (unsigned)*(unsigned short *)((char *)s + 0x4664),
                     (void *)((char *)s + 0x5024),
                     *(void **)((char *)s + 0x5028),
                     data_ov006_02140328, data_ov006_02140418[0],
                     (void *)data_ov006_02140420[0], data_ov006_02140324);
        /* the two globals that gate the ONE arm of state 020ee508 that reaches
           func_ov006_020c81e0 and installs the pair {0x020c814c, 0} -- lane
           BNT's dead-state finding, answered with a measurement. The arm needs
           02140428 nonzero AND 02140304 zero. */
        std::fprintf(stderr, "  [bnp] 020ee508 arm gate: 02140428=%d (needs "
                     "!=0)  02140304=%d (needs ==0)\n",
                     data_ov006_02140428, data_ov006_02140304);
        std::fflush(stderr);
    }
    return func_ov006_020ee034(s);
}
static void *__fastcall bnp_d2(void *s, void *)
{ BNP(16); return func_ov006_020edec0(s); }
static void *__fastcall bnp_d0(void *s, void *)
{ BNP(17); return func_ov006_020edf54(s); }

/* SLOT 19 READS ITS SECOND ARGUMENT, and this is measured rather than
   inherited. Lane LKY's note makes the forwarding question per-lane; the ROM
   body at 0x020ee8dc opens `cmp r1,#0` at 0x020ee8e4 and takes a completely
   different path when it is nonzero, so this thunk FORWARDS the argument
   rather than only popping it. src/func_ov006_020ee8dc.cpp declares the second
   parameter and reads it. */
static int __fastcall bnp_v19(void *s, void *, int sel)
{ BNP(19); return func_ov006_020ee8dc(s, sel); }

// ---- the ignition, formerly the floor --------------------------------------
//
// THE COUNTERS SURVIVE THE FLOOR THEY WERE BUILT FOR, deliberately. They were
// the trap's instrument; they are now the ignition's witness, and they answer
// a question the slot-18 hit count alone cannot: WHICH ARGUMENT the class
// ignited with. Slot 0 passes -1, so a run that reports anything else is
// reporting a second ignition from somewhere this seat has not accounted for.
//
// SM64DS_BNP_START_STATE IS GONE. It called func_ov006_020ee658 -- the ROM's
// state-0 installer -- BEHIND the trap, so the dispatch half could be shown to
// route while the round counter, the difficulty ladder, the random draw and
// the two placement calls all stayed unrun. The real body does all five, and
// leaving the diagnostic wired would install the state pair a second time
// after the ignition already installed it. Section 5 records what it was.

static unsigned g_bnp_floor_calls;
static int      g_bnp_floor_last_arg = -0x7fffffff;

static int __fastcall bnp_v18(void *s, void *, int st)
{
    BNP(18);
    ++g_bnp_floor_calls;
    g_bnp_floor_last_arg = st;
    func_ov006_020ee994(s, st);
    return 1;
}

// ---- dScMgD3DBase_c's seventeen --------------------------------------------

static int __fastcall bnp_v1(void *s, void *)
{ BNP(1); return ((MgBounceAndPounce *)s)->BeforeInitResources(); }

/* SLOT 2 DROPS ITS SECOND ARGUMENT ON THE ROM TOO. 0x020e70c0 saves r0 and
   never reads r1: it calls func_ov004_020b08f0(self) then Particle::SysTracker
   ::Initialise(self+0x47e4). The src declares one parameter, and this thunk
   pops the framework's second without forwarding it, which is what the ROM
   does. */
static int __fastcall bnp_v2(void *s, void *, unsigned f)
{ BNP(2); return port_mg_d3dbase_after_init(s, f); }

/* SLOT 5 READS ITS SECOND ARGUMENT: 0x020e6f68 is `mov r4,r1` and 0x020e6f70
   is `cmp r4,#2`, a three-way split. Forwarded. */
static int __fastcall bnp_v5(void *s, void *, unsigned b)
{ BNP(5); ((MgBounceAndPounce *)s)->AfterCleanupResources(b); return 1; }

static int __fastcall bnp_v7(void *s, void *)
{ BNP(7); return ((MgBounceAndPounce *)s)->BeforeBehavior(); }
static int __fastcall bnp_v10(void *s, void *)
{ BNP(10); return port_mg_d3dbase_before_render(s); }

/* SLOT 11 READS ITS SECOND ARGUMENT: the ROM body spins on VCOUNT and then
   TAIL-JUMPS to Scene::AfterRender(0x0202e398) with both registers riding
   through, and the src passes both explicitly. Forwarded. */
static int __fastcall bnp_v11(void *s, void *, unsigned a)
{ BNP(11); ((MgBounceAndPounce *)s)->AfterRender(a); return 1; }

static int __fastcall bnp_v24(void *s, void *)
{ BNP(24); return func_ov006_020e6e78(s); }
static int __fastcall bnp_v25(void *s, void *)
{ BNP(25); return func_ov006_020e6e54(s); }
/* SLOT 26 IS `mov r0,#2; bx lr` IN THE ROM -- eight bytes, no receiver read. */
static int __fastcall bnp_v26(void *, void *)
{ BNP(26); return func_ov006_020e6e4c(); }
/* SLOTS 27 AND 28 ARE TAIL-JUMP VENEERS whose targets DO read the receiver
   (func_ov004_020af27c reads self+0x4630 at 0x020af284, func_ov004_020af04c
   reads self+0xf4 at 0x020af060) while their src TUs declare those targets with
   no parameter. The port's tail-jump mechanism could carry that -- but this
   lane measured that port/tools/tailjump_guard.py does NOT have these two in
   its assertion set (35 frames / 22 veneer derived, unchanged from base with
   both TUs in the slice), so nothing would notice if the form stopped holding.
   Both go through the shared host copies instead, which is what lane BNT chose
   independently. */
static int __fastcall bnp_v27(void *s, void *)
{ BNP(27); port_mg_d3dbase_slot27(s); return 0; }
static int __fastcall bnp_v28(void *s, void *)
{ BNP(28); port_mg_d3dbase_slot28(s); return 0; }
static int __fastcall bnp_v29(void *s, void *)
{ BNP(29); func_ov006_020e6d24(s); return 0; }
static int __fastcall bnp_v30(void *s, void *)
{ BNP(30); func_ov006_020e6cac(s); return 0; }
static int __fastcall bnp_v31(void *s, void *)
{ BNP(31); func_ov006_020e72c0(s); return 0; }
static int __fastcall bnp_v33(void *s, void *)
{ BNP(33); func_ov006_020e7124(s); return 0; }

/* dScMgD3DBase_c's own D2 and D0, which live in slots 16 and 17 of ITS table
   only. This class overrides both, so these two run only while an object is
   dispatching through the base table -- which is a real window; see section 3. */
static void *__fastcall bnp_base_d2(void *s, void *)
{ ++g_bnp_base_hits[16]; return (void *)(size_t)_ZN17MgBounceAndPounceD1Ev(s); }
static void *__fastcall bnp_base_d0(void *s, void *)
{ ++g_bnp_base_hits[17]; return (void *)(size_t)_ZN17MgBounceAndPounceD0Ev(s); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every seat in
   this port carries, counted separately so a run can never read a no-op as the
   real body having run. */
static unsigned g_bnp_init_skipped, g_bnp_render_skipped;
static int __fastcall bnp_init_noop(void *, void *)
{ ++g_bnp_init_skipped; return 1; }
static int __fastcall bnp_render_noop(void *, void *)
{ ++g_bnp_render_skipped; return 1; }

// ---- the fill --------------------------------------------------------------

struct BnpFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, exactly like kCurlingFaces,
   kWigglerFaces and the rest, so the array is order-independent and cannot land
   on a slot the ROM did not park that body in. ONE array serves BOTH tables,
   which is safe for the same reason: a word is one address, and the eight this
   class overrides are not words the base table holds. */
static const BnpFace kJumpFaces[] = {
    /* dScMgJump_c's own eight */
    {0x020ee690u, (void *)bnp_init},   {0x020edffcu, (void *)bnp_clean},
    {0x020ee27cu, (void *)bnp_beh},    {0x020ee034u, (void *)bnp_render},
    {0x020edec0u, (void *)bnp_d2},     {0x020edf54u, (void *)bnp_d0},
    {0x020ee994u, (void *)bnp_v18},
    {0x020ee8dcu, (void *)bnp_v19},
    /* dScMgD3DBase_c's fifteen, shared with 0x175, 0x180 and 0x181 */
    {0x020e70e4u, (void *)bnp_v1},     {0x020e70c0u, (void *)bnp_v2},
    {0x020e6f60u, (void *)bnp_v5},     {0x020e7074u, (void *)bnp_v7},
    {0x020e7040u, (void *)bnp_v10},    {0x020e700cu, (void *)bnp_v11},
    {0x020e6e78u, (void *)bnp_v24},    {0x020e6e54u, (void *)bnp_v25},
    {0x020e6e4cu, (void *)bnp_v26},    {0x020e6d98u, (void *)bnp_v27},
    {0x020e6d8cu, (void *)bnp_v28},    {0x020e6d24u, (void *)bnp_v29},
    {0x020e6cacu, (void *)bnp_v30},    {0x020e72c0u, (void *)bnp_v31},
    {0x020e7124u, (void *)bnp_v33},
    /* dScMgD3DBase_c's own two destructors, in ITS table's slots 16 and 17 */
    {0x020e6c28u, (void *)bnp_base_d2},
    {0x020e6c60u, (void *)bnp_base_d0},
};

static unsigned bnp_apply(void **vt, unsigned n, const BnpFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address. A minigame table is 36 slots and every one
   of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud rather than booting. */
static unsigned bnp_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

static unsigned g_bnp_claim_base, g_bnp_claim_derived;

extern "C" void port_scene_fill_jump(void)
{
    void **base    = (void **)data_ov004_020bc0c0;
    void **d3dbase = (void **)_ZTV17MgBounceAndPounce;   /* 0x0213c62c */
    void **vt      = (void **)data_ov006_0213cbe4;

    /* dScMgBase_c's table is filled here too, for the reason
       hal/scene_mg_flower.cpp's fill states: on a tree that also carries an
       earlier minigame row this is a second pass over words that are already
       host pointers and finds nothing, because the fill keys on a DS word and
       there are none left. It is here so this class does not depend on another
       class's registry row existing -- the factory's first act is
       func_ov004_020b2adc, which writes data_ov004_020bc0c0 into the object's
       first word before either derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE, dScMgD3DBase_c. Shared arm9 words and dScMgBase_c's own
       first, then the bodies this file owns. See section 3: the object really
       does dispatch through this table during construction and teardown, and
       the count is printed so the claim is measured rather than assumed. */
    port_scene_mg_fill_shared(d3dbase, 36);
    g_bnp_claim_base = bnp_apply(d3dbase, 36, kJumpFaces,
                                 sizeof kJumpFaces / sizeof kJumpFaces[0]);

    /* THE DERIVED TABLE, dScMgJump_c. */
    port_scene_mg_fill_shared(vt, 36);
    g_bnp_claim_derived = bnp_apply(vt, 36, kJumpFaces,
                                    sizeof kJumpFaces / sizeof kJumpFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)bnp_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)bnp_render_noop;
    }

    /* SM64DS_BNP_TRACE=1: print both filled tables. A raw-DS census answers
       "did a DS word survive"; it does not answer "is any slot NULL", and a
       null slot is a jump to address zero rather than to a DS address. */
    if (std::getenv("SM64DS_BNP_TRACE")) {
        for (int t = 0; t < 3; ++t) {
            void **p = t == 0 ? base : (t == 1 ? d3dbase : vt);
            std::fprintf(stderr, "  [bnp] table %d (%s):\n",
                         t, t == 0 ? "dScMgBase_c" :
                            (t == 1 ? "dScMgD3DBase_c" : "dScMgJump_c"));
            for (int i = 0; i < 36; ++i) {
                std::fprintf(stderr, " %2d:%p", i, p[i]);
                if (i % 6 == 5) std::fprintf(stderr, "\n");
            }
            std::fflush(stderr);
        }
    }

    {
        const unsigned lb = bnp_raw_left(base, 36);
        const unsigned lm = bnp_raw_left(d3dbase, 36);
        const unsigned lv = bnp_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgD3DBase_c %u, dScMgJump_c %u. A dispatch of any "
                         "of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every seated
       minigame's fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard.

       THIS CLASS HAS NO CONSTRUCTOR OF ITS OWN AMONG THE THIRTY-FIVE, and that
       is measured rather than assumed; section 2 is the measurement. The call
       is still made, because the class's closure reaches ov006 and ov004 data
       that OTHER constructors build. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 372) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_jump_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept anyway for the reason title_spawn,
   port_mg_curling_spawn and port_mg_wiggler_spawn are: it gives the seat one
   place to observe the object without the registry table growing a column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, which is worth recording because
   0x169's did. src/MgBounceAndPounce_Spawn.cpp calls func_ov004_020b2adc(p)
   WITH its argument, where src/func_ov006_020e0574.cpp calls the same base
   constructor with none and rides r0 through. Verified against the ROM at
   0x020eeafc (0xec bytes) during this lane's adjudication pass: allocation size
   0x5834, the two 0xbc-byte camera records at this+0x466c walked by the
   0x020eeb3c..0x020eeb44 loop, Particle::SysTracker at +0x47e4, Model at
   +0x501c, three 0xb8-byte sub-objects at +0x506c built through func_020733a8
   with func_ov006_020c8a04 / func_ov006_020c893c, and six 0xf0-byte
   sub-objects at +0x5294 with func_ov006_020c6f70 / func_ov006_020c6f3c. */
extern "C" void *port_mg_jump_spawn(void)
{
    void *p = MgBounceAndPounce_Spawn();
    g_bnp_self = (char *)p;
    return p;
}

// ---- the witness -----------------------------------------------------------

extern "C" void port_scene_jump_hits(void)
{
    std::printf("[scene] dScMgJump_c slot hits: init %u, cleanup %u, behavior "
                "%u, render %u, D2 %u, D0 %u, slot18-IGNITION %u, slot19 %u\n",
                g_bnp_hits[0], g_bnp_hits[3], g_bnp_hits[6], g_bnp_hits[9],
                g_bnp_hits[16], g_bnp_hits[17], g_bnp_hits[18], g_bnp_hits[19]);
    std::printf("[scene] dScMgD3DBase_c slot hits (shared with 0x175/0x180/"
                "0x181): v1 %u, v2 %u, v5 %u, v7 %u, v10 %u, v11 %u, v24 %u, "
                "v25 %u, v26 %u, v27 %u, v28 %u, v29 %u, v30 %u, v31 %u, "
                "v33 %u; base-table D2 %u, D0 %u\n",
                g_bnp_hits[1], g_bnp_hits[2], g_bnp_hits[5], g_bnp_hits[7],
                g_bnp_hits[10], g_bnp_hits[11], g_bnp_hits[24], g_bnp_hits[25],
                g_bnp_hits[26], g_bnp_hits[27], g_bnp_hits[28], g_bnp_hits[29],
                g_bnp_hits[30], g_bnp_hits[31], g_bnp_hits[33],
                g_bnp_base_hits[16], g_bnp_base_hits[17]);
    std::printf("[scene] dScMgJump_c table claims: dScMgD3DBase_c 0x0213c62c "
                "%u slot(s), dScMgJump_c 0x0213cbe4 %u slot(s)\n",
                g_bnp_claim_base, g_bnp_claim_derived);
    if (g_bnp_init_skipped || g_bnp_render_skipped)
        std::printf("[scene] dScMgJump_c diagnostics: slot 0 skipped %u "
                    "time(s), slot 9 skipped %u time(s) -- the counts above "
                    "are NOT the real bodies\n",
                    g_bnp_init_skipped, g_bnp_render_skipped);
    {
        unsigned calls = 0, unknown = 0;
        unsigned mine = 0, hits = 0, nullpmf = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_jump_counts(&mine, &hits, &nullpmf);
        /* THE PARENTHETICAL USED TO SAY "the floor never installed one", which
           was the whole reading while slot 18 was trapped. The ignition is
           routed now, so a nonzero here means the opposite thing: the class
           dispatched before the ignition ran, or a pair got cleared under it.
           Naming what a nonzero WOULD mean is the point of printing it. */
        std::printf("[scene] dScMgJump_c state dispatch: %u reached this "
                    "class's switch, %u routed, %u with a NULL pair (with the "
                    "ignition live this must be 0 -- a nonzero is a dispatch "
                    "that beat func_ov006_020ee994), %u framework call(s), %u "
                    "UNHANDLED address(es)\n", mine, hits, nullpmf, calls,
                    unknown);
    }
    /* THE PER-ADDRESS CENSUS. A routed total and a zero UNHANDLED say every
       address the class asked for was one this seat owns; they do not say which
       of the six ran, and on a class with no state index that is the difference
       between "the machine moved" and an aggregate. A zero on a row is a
       statement about this run, not about the body. */
    {
        const unsigned n = port_mg_jump_state_count();
        std::printf("[scene] dScMgJump_c per-state census (%u routed "
                    "addresses, the whole set):\n   ", n);
        for (unsigned i = 0; i < n; ++i)
            std::printf(" %08x:%u", port_mg_jump_state_addr(i),
                        port_mg_jump_state_hit(i));
        std::printf("\n");
    }
    /* THE SHARED OBJECT MACHINE, counted separately because it is NOT this
       class's. Two dispatchers in ov006's shared minigame-object region and
       twenty-five state bodies; every dScMgD3DBase_c subclass reaches them, and
       before this seat none of the twenty-five was in any slice in the tree. A
       nonzero UNROUTED here would be a jump to a DS address. */
    {
        unsigned oc = 0, oh = 0, onull = 0;
        port_mg_objstate_counts(&oc, &oh, &onull);
        std::printf("[scene] shared object-state dispatch (dScMgD3DBase_c's "
                    "two, NOT dScMgJump_c's): %u call(s), %u routed, %u null "
                    "pair(s), %u unrouted\n",
                    oc, oh, onull, oc - oh - onull);
        const unsigned n = port_mg_objstate_count();
        std::printf("[scene] shared object-state census (%u addresses):\n   ", n);
        for (unsigned i = 0; i < n; ++i) {
            std::printf(" %08x:%u", port_mg_objstate_addr(i),
                        port_mg_objstate_hit(i));
            if (i % 8 == 7) std::printf("\n   ");
        }
        std::printf("\n");
    }
    /* STATE MOVEMENT, AS AN ADDRESS. */
    std::printf("[scene] dScMgJump_c state word at +0x5004: first 0x%08x, last "
                "0x%08x, %u change(s), %u distinct:",
                g_bnp_state_first, g_bnp_state_last, g_bnp_state_changes,
                g_bnp_state_nseen);
    for (unsigned i = 0; i < g_bnp_state_nseen; ++i)
        std::printf(" 0x%08x", g_bnp_state_seen[i]);
    std::printf(" (last code slot 6 saw: 0x%08x)\n", port_mg_jump_state_last());
    /* THE IGNITION, reported with the argument it ignited with. */
    std::printf("[scene] dScMgJump_c IGNITION func_ov006_020ee994 (vtable slot "
                "18, 0x168 bytes): ROUTED TO THE REAL BODY, entered %u time(s), "
                "last argument %d (slot 0 passes -1). Retired as a floor by run "
                "mg12 lane IGN; SM64DS_BNP_START_STATE is gone with it.\n",
                g_bnp_floor_calls,
                g_bnp_floor_calls ? g_bnp_floor_last_arg : 0);
    if (g_bnp_self)
        std::printf("[scene] dScMgJump_c object at %p, state pair {0x%08x, %d}, "
                    "round counter +0xbc = %d; difficulty ladder "
                    "data_ov006_02140328 = %d, remaining-target counter "
                    "data_ov006_02140304 = %d\n",
                    (void *)g_bnp_self,
                    *(unsigned *)(g_bnp_self + 0x5004),
                    *(int *)(g_bnp_self + 0x5008),
                    *(int *)(g_bnp_self + 0xbc),
                    data_ov006_02140328, data_ov006_02140304);
    std::fflush(stdout);
}
