/* FIVE arm9 .data TABLES THE PORT HOSTED NOWHERE -- run link100, lane ARM9T,
 * gates 225 and 225c.
 *
 * WHAT THIS IS. The same mechanism hal/actor_base_tables_ov002.cpp (gate 224)
 * and hal/w2_dtor_heads.cpp use, moved off the overlays and onto the main
 * module: release links with /OPT:REF, so a matched TU on a slice line with
 * nothing referencing it is dropped before the map is written. A ROM table the
 * port never mounted is the honest place to put the reference edge, because
 * the ROM's own words say what belongs in it.
 *
 * ALL FIVE ARE DEAD STORAGE IN THIS PORT, and that is measured rather than
 * assumed -- every ROM reader of every one of them is itself outside the link:
 *
 *   data_02099fe4  read by _ZN3IRQ13GetIRQHandlerEj (0x02056ee4, the load at
 *                  0x02056f68) and _ZN3IRQ13SetIRQHandlerEjPFvvE (0x02056f70,
 *                  the load at 0x02056ff8). BOTH are host-replaced: ntr/
 *                  runtime.cpp defines those two symbols itself under a
 *                  PORT_HOST_ABI tag that names this table, so the matched
 *                  src/ copies can never be in the link (LNK2005) and nothing
 *                  else in the image loads the address.
 *   data_02094390  ONE reader, from:0x02074fdc -> data_020a0c68 in
 *                  __sinit_02074fb8, and one consumer of that list,
 *                  func_0203506c+0x230 (dScMB_c_InitResources). Both unlinked;
 *                  func_0203506c is the 531-TU scene-boot root
 *                  port/arm9_frontier.txt ranks second.
 *   data_02092188  ONE reader, from:0x02074ecc = __sinit_02074e84+0x48, a
 *                  static initialiser that is not in the link. Independently,
 *                  hal/scene_boot.cpp's graphics-block header already says
 *                  "the Stage's own table, data_02092188, is hosted by nobody"
 *                  and that nothing seats data_0209d4a8, so the ROM's "is the
 *                  Stage's block current" test has nothing to read.
 *   data_0209a424  ONE reader and it is not code at all: from:0x0209a440, a
 *                  word inside data_0209a438, i.e. a parent descriptor points
 *                  at this table. That parent is hosted nowhere either.
 *   data_0209a744  THREE readers, all inside its own class: from:0x020736bc
 *                  (func_020736c0), 0x020736e0 (func_020736e4) and 0x020736f0
 *                  (func_020736f4). None of the three is in the link -- which
 *                  is also how this symbol is known to be unhosted, since a
 *                  linked reader would have made it an unresolved external.
 *
 * So every seat here is STORAGE, not dispatch: nothing in this build reads a
 * word back out of any of them, and the fill below therefore changes no
 * behaviour.
 * That is stated as a limit, not as a virtue -- the linkage is the whole
 * effect, and the honesty of it rests on the words being the ROM's own.
 *
 * EVERY WORD BELOW WAS READ TWICE AND THE TWO HAD TO AGREE. First
 * config/arm9/relocs.txt, every `from:` inside the table's span. Second the
 * raw image, extracted/arm9_dec.bin read flat at base 0x02004000 (the base
 * hal/scene_boot.cpp's data_0208ea6c block and port/fader_boot_map.txt section
 * 1 use), resolved through config/arm9/symbols.txt. Each target was then
 * checked to be a SYMBOL START with a real prologue, never an interior address
 * of a tail-shared body; the prologue words are quoted at each table.
 *
 * ===========================================================================
 * 1. data_02099fe4 -- THE ARM9 IRQ VECTOR TABLE, 22 SLOTS
 * ===========================================================================
 *
 * WIDTH. Next symbol in config/arm9/symbols.txt after 0x02099fe4 is
 * data_0209a03c, so the span is 0x58 = 88 bytes = 22 words. That is confirmed
 * from the other end by the ROM's own readers: src/_ZN3IRQ13GetIRQHandlerEj.c
 * and src/_ZN3IRQ13SetIRQHandlerEjPFvvE.c both walk `for (i = 0; i < 0x16;
 * i++)` -- 0x16 is 22 -- so the loop bound and the symbol delta are the same
 * number read two different ways.
 *
 * THE INDEX IS THE IRQ BIT NUMBER, not a slot ordinal: both readers shift the
 * caller's mask right one bit per step and act on bit 0. Slots 3..6 and 8..11
 * are the ones the readers DIVERT to data_020a60c4 (already hosted,
 * ntr/runtime.cpp) rather than storing here, which is why those eight are the
 * ROM bodies that forward into IRQ::DmaTimHandler and the other fourteen are
 * all the same do-nothing.
 *
 *   idx  word        symbol                            disposition
 *    0   0x02056d60  _ZN3IRQ12EmptyHandlerEv           SEATED (V_BLANK)
 *    1   0x02056d60  _ZN3IRQ12EmptyHandlerEv           same body (H_BLANK)
 *    2   0x02056d60  _ZN3IRQ12EmptyHandlerEv           same body (V_COUNT)
 *    3   0x02056c70  _ZN3IRQ19Tim0OverflowHandlerEv    SEATED
 *    4   0x02056c60  _ZN3IRQ19Tim1OverflowHandlerEv    SEATED
 *    5   0x02056c50  _ZN3IRQ19Tim2OverflowHandlerEv    SEATED
 *    6   0x02056c40  _ZN3IRQ19Tim3OverflowHandlerEv    SEATED
 *    7   0x02056d60  _ZN3IRQ12EmptyHandlerEv           same body
 *    8   0x02056cb0  _ZN3IRQ11Dma0HandlerEv            SEATED
 *    9   0x02056ca0  _ZN3IRQ11Dma1HandlerEv            SEATED
 *   10   0x02056c90  _ZN3IRQ11Dma2HandlerEv            SEATED
 *   11   0x02056c80  _ZN3IRQ11Dma3HandlerEv            SEATED
 *   12..21  0x02056d60  _ZN3IRQ12EmptyHandlerEv        same body
 *
 * NINE DISTINCT BODIES, and all nine are the residue rows this root names.
 * Their prologues out of the image, which is what says they are entries and
 * not interior addresses:
 *
 *   0x02056c40  e59fc004 e3a00007 e12fff1c   ldr ip,[pc,#4]; mov r0,#7; bx ip
 *   0x02056cb0  e59fc004 e3a00000 e12fff1c   ldr ip,[pc,#4]; mov r0,#0; bx ip
 *   0x02056d60  e12fff1e                     bx lr        (size 0x4, empty)
 *
 * config/arm9/symbols.txt sizes them 0x10, 0x10 and 0x4, so each of the eight
 * forwarders is exactly the three instructions plus the literal it loads.
 *
 * A TENTH TU RIDES IN ON THEM. All eight forwarders tail into 0x02056cc0,
 * which config/arm9/symbols.txt names _ZN3IRQ13DmaTimHandlerEv and which the
 * eight src TUs spell func_02056cc0 -- one address, two names. That is a
 * COMPILE_DEFINITIONS row in port/CMakeLists.txt, not an /alternatename: the
 * gate-224 block's technique, and it keeps tools/alternatename_guard.py out of
 * it entirely. The body is src/_ZN3IRQ13DmaTimHandlerEv.c and it is the tenth
 * line of port/slice_gate225.txt.
 *
 * ITS THREE GLOBALS WERE CHECKED BEFORE IT WAS SLICED, because a handler that
 * links and then writes past a host object is worse than one that does not
 * link -- port/irq2_map.txt section 8 is that exact story about this exact
 * body:
 *   data_020a60c4   hosted, ntr/runtime.cpp:156, 8 x 12 bytes; the src's
 *                   DmaEnt {fn, flag, arg} is the same 12-byte shape word for
 *                   word.
 *   data_023c0000   hosted, hal/player_bridges.cpp:2365, 0x4000 bytes. It is
 *                   written at +0x3ff8 (the BIOS interrupt-check flag at
 *                   DTCM_END-8) and irq2_map section 8 records that this body
 *                   is one of the five writers the old 64-byte sizing would
 *                   have run off the end of. The object is 16 KB now, so the
 *                   store lands inside it.
 *   data_02099fd4   NOT hosted anywhere, and hosted below.
 *
 * data_02099fd4 IS THE BIT-NUMBER TABLE THAT HANDLER INDEXES. Span 0x02099fd4
 * to 0x02099fe4 (next symbol) = 16 bytes, and NO relocation in
 * config/arm9/relocs.txt lands inside it, so the ROM bytes are the whole
 * truth. Read as eight u16 out of the image:
 *
 *   8, 9, 10, 11, 3, 4, 5, 6
 *
 * which is DMA0..3 then Timer0..3 -- the same eight IRQ bit numbers the vector
 * table above diverts, in the same order data_020a60c4 keeps them. It carries
 * its ROM values as an initialiser; nothing needs a reference edge INTO it
 * because DmaTimHandler is what names it.
 *
 * WHY THE HOST'S IRQ STAND-INS ARE NOT REWIRED ONTO THIS TABLE, which is the
 * obvious next question and the answer is a hazard, not an oversight.
 * ntr/runtime.cpp models two masks with two file statics (g_gxfifo_handler for
 * 0x200000, g_hblank_handler for mask 2) and drops every other mask. Moving
 * that storage into data_02099fe4[21] and [1] would look strictly more
 * faithful and would BREAK THE HBLANK GATE: ntr::rt_hblank_gates() reports
 * HBLANK_GATE_HANDLER from `if (g_hblank_handler)`, and this table's ROM value
 * at index 1 is EmptyHandler, not zero. A seat would hand that gate a
 * permanently-open answer and let rt_hblank_dispatch() call EmptyHandler once
 * the other four gates open -- on the wipe path port/irq2_map.txt sections 2
 * and 8 are about. The ROM does not gate on non-nullness at all (it gates on
 * IE bit 1 and DISPSTAT bit 4), so the honest version of that rewiring is a
 * change to the GATE and not just to the storage, and it belongs to whoever
 * owns the ntr runtime. Left alone deliberately, and named here so the next
 * lane does not have to rediscover it.
 *
 * ===========================================================================
 * 3. data_02092188 -- THE STAGE'S GRAPH CALLBACK TABLE, 4 SLOTS
 * ===========================================================================
 *
 * WIDTH. Next symbol after 0x02092188 is data_02092198: 0x10 = 4 words. Same
 * shape and the same job as section 2's, one class up: the ROM's scene manager
 * keeps the current graphics block in data_0209d4a8 and calls its slots from
 * the frame beat.
 *
 *   idx  word        symbol                        disposition
 *    0   0x02018eb8  _ZN5Scene14GraphCallback0Ev   already linked
 *    1   0x02029838  _ZN5Stage14GraphCallback1Ev   SEATED, was unlinked
 *    2   0x020297f4  _ZN5Stage14GraphCallback2EP12SceneRelated   TRAPS
 *    3   0x02018ea0  _ZN5Scene14GraphCallback3Ev   already linked
 *
 * SLOT 2 IS THE ONE ROW THIS TABLE COULD NOT BUY, and the reason is an
 * EXISTING RULING rather than this gate's caution. port/slice_w8a.txt: "NOT
 * HOSTABLE AT ALL. Stage::GraphCallback2's whole body takes the address of
 * reg_G2S_DB_BG3PA and MSVC cannot put a global at 0x04001030."
 * hal/sub_actors.cpp's port_minimap_affine_update is the stand-in and passes
 * the literal 0x04001030 instead. port/tools/closure.py confirmed it
 * independently before the slice line was written: _reg_G2S_DB_BG3PA came back
 * as the one unresolved external of that TU. So the slot traps.
 *
 * THE THREE `return 1` BODIES ARE ONE BODY IN THIS IMAGE. GraphCallback0, 1
 * and 3 are all `return 1` and /OPT:ICF folds them: the baseline map has
 * __ZN5Scene14GraphCallback0Ev, __ZN5Scene14GraphCallback1Ev and
 * __ZN5Scene14GraphCallback3Ev at one address, 0x00431300. Stage's slot 1 is
 * NOT one of those -- _ZN5Stage14GraphCallback1Ev calls Particle::RenderAll
 * and then returns 1, which is why it is a row and the Scene ones are not.
 *
 * ===========================================================================
 * 4. data_0209a424 -- A CALLBACK PAIR, 2 SLOTS
 * ===========================================================================
 *
 * WIDTH. Next symbol after 0x0209a424 is data_0209a42c: 0x8 = 2 words. Both
 * relocated:
 *
 *   idx  word        symbol          prologue                    disposition
 *    0   0x0206e254  func_0206e254   e3510000 03a00000 012fff1e   SEATED
 *    1   0x0206e240  func_0206e240   e3500000 03a00000 15c01000   SEATED
 *
 * Two small leaf predicates -- `int f(unsigned char *p, unsigned char v)` and
 * `int f(unsigned short *dst, unsigned char *src, int flag)` in src/, each a
 * null check and a store -- with no callees at all, which is why the closure
 * of this table is exactly its own two TUs.
 *
 * ===========================================================================
 * 5. data_0209a744 -- A TWO-SLOT VTABLE, HOSTED 4 WORDS WIDE
 * ===========================================================================
 *
 * WIDTH. Next symbol after 0x0209a744 is data_0209a754: 0x10 = 4 words. But
 * only TWO of those are this class's slots, for hal/scene_boot.cpp's
 * data_0208ea6c reason exactly: an Itanium vtable symbol starts at the first
 * virtual, so the two words after the last slot are the NEXT table's header.
 *
 *   idx  word        symbol           disposition
 *    0   0x020736e4  func_020736e4    SEATED, was unlinked
 *    1   0x020736c0  func_020736c0    TRAPS -- see below
 *    2   0x00000000  --               the next table's offset-to-top, the one
 *                                     word in the run with no relocation
 *    3   0x0209a708  data_0209a708    the next table's typeinfo, a DS data
 *                                     address and not code
 *
 * The array is four words because that is the symbol's span and a save state
 * captures by span; slots 2 and 3 are left null rather than carrying a DS
 * address a host cannot mean anything by, and nothing dispatches them.
 *
 * SLOT 1 TRAPS BECAUSE IT IS NOT THIS LANE'S. func_020736c0 is a row of run
 * link100's .exceptix census, not of the residue this lane was given, and a
 * seat that reaches into another lane's list is how two lanes land the same TU
 * twice. Its ROM word is real and is written down here so that lane does not
 * have to find it again.
 *
 * THE SEAT IS SELF-REFERENTIAL AND THAT IS THE ROM'S SHAPE, not a trick:
 * src/func_020736e4.c is the whole body `void func_020736e4(int *p) { p[0] =
 * (int)data_0209a744; }`, a base-subobject destructor that writes its own
 * class's vptr. So the table names the body and the body names the table. The
 * reference edge that keeps it alive is the fill below, which is CODE in an
 * object the link always carries, not the data word.
 *
 * ===========================================================================
 * RULED OUT AND WHY: data_020927ec, the third table this lane was handed
 * ===========================================================================
 *
 * It is a real table and its words were read: 7 slots (0x020927ec ->
 * data_02092808, 0x1c / 4), with func_02030dac, func_02030ca0, func_02030c38
 * and func_02030b58 at 1, 3, 4 and 5 and the ROM's own zeros at 0, 2 and 6.
 * Its one reader is func_02030e4c+0x158, and src/func_02030e4c.c ends
 * `fn = data_020927ec[data_0209fc68]; if (fn != 0) fn();` -- plain
 * `void (*)(void)` slots with no receiver, so it would have been the EASIEST
 * of the three to seat and needs no veneers at all.
 *
 * WHAT STOPPED IT IS THE CLOSURE, resolved symbol by symbol against this
 * build's own build/port/walk_window.map rather than guessed: the four bodies
 * require 40 unlinked TUs and 17 arm9 data symbols the port hosts nowhere.
 * The path is func_0203d950 and func_02030790 into the wireless/WM stack
 * (WM_SendCommand, WM_SetCallbackTable, Wireless_Reset and their thirty-odd
 * companions), and ONE of the 40 is src/_ZN3IRQ13SetIRQHandlerEjPFvvE.c --
 * an LNK2005 against ntr/runtime.cpp's host copy of that symbol on sight, and
 * the same host copy section 1 above is about.
 *
 * That is a lane, not a line, and it is a much bigger one than its four
 * census rows suggest -- the 40 TUs are all matched and all unlinked, so
 * whoever hosts those 17 data symbols is paid 40 rows for it. Left for them,
 * with the closure written down so it does not have to be measured twice.
 *
 * ===========================================================================
 * 2. data_02094390 -- dScMB_c's GRAPH DESCRIPTOR, 4 SLOTS
 * ===========================================================================
 *
 * WIDTH. Next symbol after 0x02094390 is data_020943a0: 0x10 = 4 words.
 *
 *   idx  word        symbol                        disposition
 *    0   0x02034d2c  func_02034d2c                 SEATED (int f(void), ret 0)
 *    1   0x02018eb0  _ZN5Scene14GraphCallback1Ev   already linked
 *    2   0x02034d24  func_02034d24                 SEATED (int f(void), ret 0)
 *    3   0x02034b40  func_02034b40                 SEATED (int f(char *self))
 *
 * SLOT 3 TAKES A RECEIVER AND THE OTHER THREE DO NOT, which is read off the
 * bodies rather than off the table: src/func_02034b40.c is `int
 * func_02034b40(char *self)` and reads self+4, +8, +0xc and +0xd, while
 * func_02034d24 / func_02034d2c are two-instruction `mov r0,#0; bx lr` bodies
 * and Scene::GraphCallback1 is `mov r0,#1; bx lr`. The seat below stores the
 * bodies' own addresses and does NOT wrap slot 3 in a __fastcall veneer,
 * because -- see the header of this section -- nothing in this link dispatches
 * this table, so there is no call site whose convention a veneer would have to
 * match. If a lane ever links func_0203506c, that lane owns the veneer
 * question and hal/scene_boot.cpp's data_0208ea6c block is the worked example.
 *
 * func_02034b40 PULLS func_02034d34 IN BEHIND IT (the census lists it as
 * from-unlinked-only on func_02034b40), so this three-word seat is worth four
 * TUs; every other callee it names is already in the link, which is what
 * port/tools/closure.py was run to establish before the slice line was added.
 *
 * WHAT IS NOT HERE. __sinit_02074fb8 -- the static initialiser that stores
 * &data_02094390 into data_020a0c68 -- is a residue row of its own and is
 * DELIBERATELY not sliced. Adding it would put a pre-main write to
 * data_020a0c68 into every target, which is a behaviour change (the ROM runs
 * that sinit from the .ctor chain during a boot this port does not perform),
 * and the linkage it buys is one TU. A seat that changes what the port does at
 * startup to buy one row is the wrong trade.
 *
 * ===========================================================================
 * HOW THE FILL GETS CALLED, and why the arrays are not just initialised
 * ===========================================================================
 *
 * A namespace-scope object with a constructor, exactly as
 * hal/actor_base_tables_ov002.cpp does it and for its reason: the seats every
 * wave-1 lane landed are called by name from hal/level_boot.cpp, this lane
 * does not own that file, and the campaign's rule is that a lane never edits
 * another lane's file to add a call. A pre-main write is only safe for a table
 * no runtime fill also writes, and neither of these two is written anywhere
 * else in the port -- which is the same measurement that says they are dead
 * storage. The fill keeps external C linkage so a later change that does own
 * the boot spine can call it explicitly and drop the object.
 *
 * The arrays are left uninitialised and filled at run time rather than given
 * static initialisers, again on gate 224's shape: a static initialiser puts
 * the reference edge in the DATA section, and the two blocks in
 * port/CMakeLists.txt at the ov006 pair-word mounts record that mounted data
 * holding DS addresses is exactly the case where /OPT:REF drops the bodies
 * anyway. A fill called from a CRT initialiser puts the edge in CODE the
 * linker cannot reach past.
 */
#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"

/* ONE LINK SEAM, and it is the whole cost of section 2's slot 3.
 *
 * src/func_02034b40.c declares its callee `extern void _ZN3OAM5ResetEv(void);`
 * -- the Itanium C name, with C linkage. The one real definition of that body
 * is src/_ZN3OAM5ResetEv.cpp, WHICH IS ALREADY IN THIS LINK, and it spells the
 * function `namespace OAM { void Reset(void) }`, so MSVC mangles it
 * ?Reset@OAM@@YAXXZ (map row 19051 of this gate's baseline link, object
 * _ZN3OAM5ResetEv.cpp.obj). One body, two spellings, no host copy anywhere --
 * exactly the shape hal/cxx_aliases.cpp's Sound::PlayCharVoice row bridges,
 * and the same direction as its sixty `_func_XXXXXXXX=<other C name>` rows.
 * Void, no arguments, __cdecl on both sides, so the alias carries no
 * convention change; port/ov007_seat.txt's ruling that an alias cannot change
 * a calling convention is satisfied by there being no difference to change.
 *
 * IT IS HERE AND NOT IN hal/cxx_aliases.cpp on purpose: the LHS is undefined
 * everywhere in this build (nothing defines a C `_ZN3OAM5ResetEv`), so
 * port/tools/alternatename_guard.py's DEFEATED test cannot fire on it, and
 * keeping the directive next to the one slot that needs it means a later lane
 * that drops that slot drops the alias with it. */
#pragma comment(linker, "/alternatename:__ZN3OAM5ResetEv=?Reset@OAM@@YAXXZ")

extern "C" {
/* ---- 1. the nine IRQ handler bodies. Every one is a src/ TU on
   port/slice_gate225.txt; none is declared into existence here. ---- */
void _ZN3IRQ12EmptyHandlerEv(void);          /* src/_ZN3IRQ12EmptyHandlerEv.c */
void _ZN3IRQ19Tim0OverflowHandlerEv(void);
void _ZN3IRQ19Tim1OverflowHandlerEv(void);
void _ZN3IRQ19Tim2OverflowHandlerEv(void);
void _ZN3IRQ19Tim3OverflowHandlerEv(void);
void _ZN3IRQ11Dma0HandlerEv(void);
void _ZN3IRQ11Dma1HandlerEv(void);
void _ZN3IRQ11Dma2HandlerEv(void);
void _ZN3IRQ11Dma3HandlerEv(void);

/* ---- 2. dScMB_c's graph descriptor. GraphCallback1 is already in the link;
   the other three come in on port/slice_gate225.txt. ---- */
int func_02034d2c(void);
int _ZN5Scene14GraphCallback1Ev(void);
int func_02034d24(void);
int func_02034b40(char *self);

/* ---- 3. the Stage's graph callbacks. 0 and 3 are already in the link (and
   /OPT:ICF-folded onto each other); 1 comes in on port/slice_gate225.txt. ---- */
int _ZN5Scene14GraphCallback0Ev(void);
int _ZN5Stage14GraphCallback1Ev(void);
int _ZN5Scene14GraphCallback3Ev(void);

/* ---- 4 and 5 ---- */
int  func_0206e254(unsigned short *dst, unsigned char *src, int flag);
int  func_0206e240(unsigned char *p, unsigned char v);
void func_020736e4(int *p);
}

/* The two slots whose ROM word names a body this gate does not seat. Both are
   named rather than shared with a generic trap, because WHICH slot fired is
   the whole content of the report -- the shape hal/scene_boot.cpp's l2_trap
   and hal/actor_base_tables_ov002.cpp's ob2_trap_report both take. Neither can
   fire in this build: nothing dispatches either table (see the header). */
static void a9t_trap(const char *what)
{
    std::fprintf(stderr, "  UNHOSTED: %s\n", what);
    std::fflush(stderr);
    if (std::getenv("SM64DS_FAULTS_FATAL"))
        std::exit(3);
}
static int a9t_trap_2188_s2(void)
{
    a9t_trap("data_02092188 slot 2, Stage::GraphCallback2 -- not hostable, it "
             "takes the address of reg_G2S_DB_BG3PA at 0x04001030");
    return 1;
}
static void a9t_trap_a744_s1(void)
{
    a9t_trap("data_0209a744 slot 1, func_020736c0 -- an .exceptix census row, "
             "not this lane's to seat");
}

/* THE HOST ARRAYS. DS-shaped names, so they sit inside the .dsstate span a
   save state captures or port/tools/dsstate_guard.py fails the build. Widths
   are the next-symbol delta out of config/arm9/symbols.txt, quoted per table
   in the header above. */
extern "C" {
DSSTATE_BEGIN
void *data_02099fe4[22];   /* 0x02099fe4 -> data_0209a03c, 0x58 / 4 */
void *data_02094390[4];    /* 0x02094390 -> data_020943a0, 0x10 / 4 */
void *data_02092188[4];    /* 0x02092188 -> data_02092198, 0x10 / 4 */
void *data_0209a424[2];    /* 0x0209a424 -> data_0209a42c, 0x08 / 4 */
void *data_0209a744[4];    /* 0x0209a744 -> data_0209a754, 0x10 / 4, 2 slots */
/* the one table here that is pure ROM data: eight IRQ bit numbers, no
   relocation anywhere inside 0x02099fd4..0x02099fe4, carried verbatim. */
unsigned short data_02099fd4[8] = { 8, 9, 10, 11, 3, 4, 5, 6 };
DSSTATE_END
}

extern "C" void hal_seat_arm9_link100_tables(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    /* 1. data_02099fe4 -- the IRQ vector table, index == IRQ bit number. */
    {
        void **vt = data_02099fe4;
        for (int i = 0; i < 22; ++i)
            vt[i] = (void *)_ZN3IRQ12EmptyHandlerEv;   /* the ROM's default */
        vt[3]  = (void *)_ZN3IRQ19Tim0OverflowHandlerEv;
        vt[4]  = (void *)_ZN3IRQ19Tim1OverflowHandlerEv;
        vt[5]  = (void *)_ZN3IRQ19Tim2OverflowHandlerEv;
        vt[6]  = (void *)_ZN3IRQ19Tim3OverflowHandlerEv;
        vt[8]  = (void *)_ZN3IRQ11Dma0HandlerEv;
        vt[9]  = (void *)_ZN3IRQ11Dma1HandlerEv;
        vt[10] = (void *)_ZN3IRQ11Dma2HandlerEv;
        vt[11] = (void *)_ZN3IRQ11Dma3HandlerEv;
    }

    /* 2. data_02094390 -- dScMB_c's graph descriptor. */
    data_02094390[0] = (void *)func_02034d2c;
    data_02094390[1] = (void *)_ZN5Scene14GraphCallback1Ev;
    data_02094390[2] = (void *)func_02034d24;
    data_02094390[3] = (void *)func_02034b40;

    /* 3. data_02092188 -- the Stage's. Slot 2 is not hostable and traps. */
    data_02092188[0] = (void *)_ZN5Scene14GraphCallback0Ev;
    data_02092188[1] = (void *)_ZN5Stage14GraphCallback1Ev;
    data_02092188[2] = (void *)a9t_trap_2188_s2;
    data_02092188[3] = (void *)_ZN5Scene14GraphCallback3Ev;

    /* 4. data_0209a424 -- the callback pair. */
    data_0209a424[0] = (void *)func_0206e254;
    data_0209a424[1] = (void *)func_0206e240;

    /* 5. data_0209a744 -- two slots; 2 and 3 are the next table's header and
       stay null, and slot 1 belongs to the .exceptix census. */
    data_0209a744[0] = (void *)func_020736e4;
    data_0209a744[1] = (void *)a9t_trap_a744_s1;
}

namespace {
struct Arm9Link100TableSeat {
    Arm9Link100TableSeat() { hal_seat_arm9_link100_tables(); }
};
Arm9Link100TableSeat g_arm9_link100_table_seat;
}  // namespace
