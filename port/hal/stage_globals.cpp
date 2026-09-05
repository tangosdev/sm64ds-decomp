// ---- THE DS GLOBALS Stage::Behavior'S OWN CLOSURE READS ------------------
//
// (run link100, lane FRAME. port/slice_gate220.txt is the enrolment,
// port/stage_lifecycle_map.txt section 13 the accounting.)
//
// Seating _ZTV5Stage slot 6 puts the ROM's own Stage::Behavior in the frame,
// and with it fifty-odd matched TUs that had never been in a link: the pause
// screen (PS_Init / PS_Update and its five menu arms), the level-clear arm
// (LC_Update), the VS-exit pair and the Message::Display* family they draw
// through. Those bodies read eighteen arm9 .bss words that no host file had
// hosted, because nothing had ever reached them.
//
// WHY HERE AND NOT hal/auto_bss.cpp. auto_bss.cpp is that file's own lane's,
// and its rows are generated; these eighteen are a hand-picked set with one
// reason -- "Stage::Behavior's closure reads it" -- and they belong beside the
// seat that made them reachable, in this lane's own hal/stage_*.cpp. If a
// later pass folds them into auto_bss.cpp's generated block, nothing here is
// load-bearing beyond the sizes.
//
// SIZED BY ROM SPAN, NOT BY THE FIELD THE CALLER READS. Every size below is
// the delta from the symbol's own address to the NEXT symbol in
// config/arm9/symbols.txt, computed at enrolment time rather than guessed from
// the widest access seen in src/. That is the rule
// port/status/undersized-globals wrote after a host global sized by its first
// caller took an interior write from its second: a four-byte host object for a
// symbol whose ROM span is 1200 bytes reads correct for as long as nobody
// walks it.
//
//   data_0209d4ec     4     the message/save block
//   data_0209d4f4     4
//   data_0209d674     4     Message::DisplaySaveStatusText's own word
//   data_0209fc6c     4  \
//   data_0209fc74     4   |
//   data_0209fc8c     4   |  the VS-exit block: func_020321fc, func_020326ac
//   data_0209fcac     4   |  and func_020345b0 read all eleven, and
//   data_0209fcb4     4   |  Stage::Behavior reaches them through
//   data_0209fcb8     4   |  func_02032f54 on the VS side only
//   data_0209fcc0     4   |
//   data_0209fce0     4   |
//   data_0209fce4     4   |
//   data_0209fcec     4   |
//   data_0209fcf0     4  /
//   data_020a5538    64     the three the allocator's own accounting reads,
//   data_020a5578   128     reached from func_02019a58. NOT the allocator
//   data_020a5bd4  1200     itself -- that subtree is the seam below.
//   data_020a8110     4
//
// NONE OF THE EIGHTEEN IS SEEDED, and that is deliberate rather than an
// omission. Every one is arm9 .bss -- `kind:bss` in config/arm9/symbols.txt --
// which the DS zeroes before main and which the ROM's own code then fills.
// A host file that wrote a value into one of them would be writing Nintendo's
// number down, which is the thing romdata.py exists to avoid.

#include "dsstate_seg.h"

/* ---- CAPTURED. Every one of the eighteen is DS STATE -- arm9 .bss the
 * game writes and reads across frames -- so all eighteen sit inside the
 * .dsstate span a save state rolls back. port/tools/dsstate_guard.py is
 * what says so rather than this comment: it runs post-link over
 * walk_window.map and refuses a build where a hosted DS symbol landed
 * outside the captured range. It refused this file once, before the
 * DSSTATE_BEGIN below existed, and named all eighteen.
 *
 * __aeabi_uidiv is deliberately OUTSIDE the block: it is host code with no
 * state, not a DS word. */
extern "C" {

DSSTATE_BEGIN

/* the message / save-status words */
unsigned char data_0209d4ec[4];
unsigned char data_0209d4f4[4];
unsigned char data_0209d674[4];

/* the VS-exit block */
unsigned char data_0209fc6c[4];
unsigned char data_0209fc74[4];
unsigned char data_0209fc8c[4];
unsigned char data_0209fcac[4];
unsigned char data_0209fcb4[4];
unsigned char data_0209fcb8[4];
unsigned char data_0209fcc0[4];
unsigned char data_0209fce0[4];
unsigned char data_0209fce4[4];
unsigned char data_0209fcec[4];
unsigned char data_0209fcf0[4];

/* the allocator-accounting words func_02019a58 reaches */
unsigned char data_020a5538[64];
unsigned char data_020a5578[128];
unsigned char data_020a5bd4[1200];
unsigned char data_020a8110[4];

DSSTATE_END

/* ---- __aeabi_uidiv ------------------------------------------------------
 *
 * PORT_HOST_ABI: ARM EABI unsigned-divide helper. The DS's ARM946 has no
 * divide instruction, so the pinned compiler emits a call to this ITCM
 * primitive for every `/` and `%` on unsigned operands; MSVC emits a native
 * div and never needs it, so nothing in this tree defined it until a matched
 * body that DOES divide came into the link.
 *
 * THE SIBLING IS ALREADY HERE AND THIS IS THE SAME TREATMENT.
 * hal/actor_vtables.cpp:564 carries __aeabi_idiv, the signed half, with the
 * same one-line body and the same reasoning; walk_window.map has carried
 * ___aeabi_idiv since. This is its unsigned twin, reached from func_0204fce8
 * in Stage::Behavior's closure.
 *
 * THE ZERO GUARD IS THE HOST'S, NOT THE ROM'S. ARM's helper returns 0 for a
 * zero divisor; x86's div raises #DE and takes the process down. The guard
 * reproduces the ARM answer rather than the x86 one, which is what a faithful
 * primitive does -- and it matches __aeabi_idiv's own body exactly.
 *
 * ONE RETURN VALUE, NOT TWO, and the difference from func_01ffadf0 in
 * hal/scene_vs_menu.cpp is worth naming so a reader does not "fix" one into
 * the other. That file hosts the ITCM ADDRESS 0x01ffadf0 as a `long long`
 * because ov075's own caller (src/func_ov075_021143e4.cpp) declares it that
 * way to read the quotient AND the remainder out of the r0/r1 pair. Every
 * caller of the NAME __aeabi_uidiv in this tree declares it
 * `unsigned int (unsigned int, unsigned int)` -- src/func_0204fce8.c,
 * src/func_02041224.c, src/_ZN9Butterfly13InitResourcesEv.cpp -- and reads
 * the quotient only. Two spellings, two declared shapes, both correct for
 * their own callers. */
unsigned int __aeabi_uidiv(unsigned int n, unsigned int d)
{
    return d ? n / d : 0u;
}

}   /* extern "C" */
