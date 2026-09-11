// THE ROM'S WIRELESS WORKER THREAD, RUNNING AS THE ROM WROTE IT.
// Run link100, lane WM5, rung W6 of the wireless plan.
//
// ============================ WHAT THIS RUNG IS ============================
//
// src/func_02040c34.c is the last body on the VS path that the port still
// answered with a stand-in. hal/comms_seam.cpp's note called it "starts the
// DS's wireless THREAD with two callbacks" and recorded that the host had no
// thread to start. Both halves of that have since become false:
//
//   * gate 223 (run link100, lane THREAD) made src/func_02058200.c -- the
//     ROM's own OS_CreateThread -- a linked body, with hal/boot2_thread.cpp
//     adopting any record it creates onto a Windows fiber. The card driver's
//     thread has run that way since, and port/tools/thread_create_proof.py
//     measures it.
//   * lanes NITROFS and LOADOV wrote the cartridge-header mirror at
//     0x027FFE40..0x027FFE5C, all eight words, read back through the ROM's own
//     FS_Init. src/func_020424c0.c reads exactly those four (offset, size)
//     pairs, and until they existed it had nothing to read.
//
// So this rung deletes the stand-in and lets the cartridge do it. What runs
// now, in the ROM's own instructions, when the lockstep opens a session:
//
//   src/func_0203ea5c.c:209/212  func_02040c34(role, 1, &func_0203f644,
//                                              &func_0203f604, 0)
//     -> IRQ::Disable, the once-flag at data_020a0f84
//     -> the 0x440 control block at data_020a1fc0 filled from the arguments
//     -> func_02061c88(port, func_020417d0, 0)  the WM port callback, which
//        needs WM_Init to have run and it has (rung W1, lane WM2)
//     -> func_0205d89c(-1) -> func_0205d96c, the cartridge-header mirror
//     -- PARENT (role != 0):
//     -> three 0xcc0-byte work nodes carved out of data_020a2400 + 0xc0, each
//        with FS_InitFile on its +0x38 file handle, chained into the free list
//        at +0x2700
//     -> func_020652fc (FACED, see below), func_020424c0 (the four header
//        regions read over the fs seam)
//     -> one node dequeued, zeroed, pushed with func_020658c0
//     -> func_02042200 -> func_02058200(record=data_020a15e4,
//                          entry=func_02042254, arg=data_020a11e4,
//                          stack top=data_020a15e4, size=0x400, prio=0xf)
//        then func_02058048 to make it runnable. THE SECOND CREATED THREAD.
//     -- CHILD (role == 0):
//     -> the sixteen-slot table at data_020a2400 + 0xc0, func_02041224 twice,
//        func_020424c0, func_02065234 (FACED). No thread on this arm; the ROM
//        does not create one for a child.
//
// The worker itself is src/func_02042254.c and its shape is a plain consumer:
//
//     while (1) {
//         irq = IRQ::Disable();
//         while ((node = *(base + 0x2708)) == 0) {
//             if (*(data_020a1fc0 + 0xc) == 0) { IRQ::Restore(irq); return; }
//             OS_SleepThread(0);            // 020422D0, argument r6 = 0
//         }
//         IRQ::Restore(irq);
//         ... FS_ReadFile three 0x400 pages into the node, or FS_CloseFile ...
//     }
//
// WHAT WAKES IT, and this is the question the brief asked to be answered
// exactly. NOT an interrupt. OS_SleepThread(0) is passed a NULL queue at
// 0x020422CC/D0, so no OSThreadQueue anywhere holds this thread's id bit and
// no OS_WakeupThread call in the cartridge can reach it (checked by scanning
// every BL to 0x0205807c in extracted/arm9_dec.bin: thirteen callers, none of
// them on this chain). The wake is a DIRECT one: src/func_020423c8.c is
// `func_02058048(data_020a15e4)` -- state = 1 on this exact record, then the
// ROM's own reschedule -- and it is called from src/func_020412f0.c, the
// message callback, on the arm that has just moved a node onto the +0x2708
// list with func_02041ae8. Same function func_02042200 uses to start the
// thread in the first place. So the host needs no interrupt source it does not
// have: it needs the ROM's own scheduler, which it has.
//
// ON RUNG W6 THE WAKE COULD NOT FIRE, and the reason was this file: the note
// that stood here said "func_020412f0 is installed as the 0x02065xxx layer's
// callback by func_020652fc, which is FACED below, so nothing invokes it".
// RUNG W7 RETIRES THAT FACE. src/func_020652fc.c is a linked ROM body now, so
// the callback really is installed -- func_02040c34 loads func_020412f0 into
// r2 at 0x02040DE0 and the ROM's own initialiser stores it. What still does
// not happen is the message that would make it run: func_020412f0 is invoked
// from the 0x02065xxx layer's receive path, which needs an 0x82 port-receive
// indication from the host ARM7, and the host posts none. So the worker is
// created, entered, and parks in the ROM's own sleep -- the same reading rung
// W6 measured, now for one reason instead of two.
//
// ==================== WHAT RUNG W6 REFUSED, AND WHY W7 DID NOT =============
//
// Rung W6 left ELEVEN TUs of the 0x02063E-0x020655 table band behind four
// faces here. The refusal, in its own words, was that the band is reached only
// through two ROM accessors that return LITERAL DS ADDRESSES:
//
//     src/func_02065b94.c   return 34247892;   // read as 0x020A9594
//     src/func_02065b88.c   return 34248048;   // read as 0x020A9630
//
// and that src/func_020652fc.c's walk "reaches 0x020A9D83, past
// data_020a9570's span and across three further ROM symbols", so that linking
// the band would leave one ROM object as two host objects.
//
// BOTH ADDRESSES IN THAT SENTENCE WERE A DECIMAL MISREAD, and rung W7 (run
// link100, lane WM8) is the correction:
//
//     34247892 = 0x020A94D4      not 0x020A9594
//     34248048 = 0x020A9570      not 0x020A9630
//
// The cartridge says so twice. The two literal pools at 0x02065B9C and
// 0x02065B90 hold d4940a02 and 70950a02, and config/arm9/relocs.txt carries a
// NAMED relocation for each of them:
//
//     from:0x02065b9c kind:load to:0x020a94d4 module:main
//     from:0x02065b90 kind:load to:0x020a9570 module:main
//
// So func_02065b88 returns the address of data_020a9570 EXACTLY -- the object
// hal/wm_arm7.cpp has hosted since rung W4 -- and func_02065b94 returns the
// address of data_020a94d4, the 0x9c bytes immediately in front of it, which
// nothing hosted until rung W7. The two together are ONE 0x854-byte ROM
// object, 0x020A94D4..0x020A9D28, and the ROM's own sixteen-iteration loop
// measures it: p + 0x1d4 + 16 * 0x68 = 0x020A9D28, which is the next symbol in
// config/arm9/symbols.txt, exactly. The furthest byte that loop writes is
// 0x020A9CC3, inside data_020a9570 and nowhere near 0x020A9D83. There was no
// write past the span and there were no three symbols to cross.
//
// WHAT RUNG W7 DID, AND THE ONE THING IT DELIBERATELY DID NOT DO. It hosts
// data_020a94d4 in hal/wm_arm7.cpp's own grouped run, immediately in front of
// data_020a9570 (".dsstate$ywmd18a"), so the 0x854 bytes are one contiguous
// host object in the ROM's order; and it answers the two accessors with that
// object's HOST address in exactly the nine TUs that call them, by a
// per-source rename in port/CMakeLists.txt onto port_wm8_wmq_base and
// port_wm8_wmq_head in hal/wm_arm7.cpp. Those two CALL the ROM body and
// translate its answer by offset, so the cartridge's own value is what the
// host address is computed from; the note at those two functions has the
// measurement that forced that shape. The eleven TUs are on
// port/slice_wm8.txt and the four faces below are gone.
//
// IT DID NOT PUBLISH THE SPAN AT 0x020A94D4 IN DS MAIN RAM, which is the other
// mechanism the port owns -- hal/ctor_runner.cpp puts the .ctor table at the
// ROM's own 0x02086b60 because a matched TU reads that address as a literal
// and a host symbol is invisible to such a load. Four reasons, in the order
// they decide it:
//
//   1. THAT MECHANISM BINDS POINTERS; THIS BAND IS STORAGE. ctor_runner's
//      object is a table of code addresses: it is written once at load with
//      each ROM word bound to the host body that answers for it, and after
//      that only READ, through the literal, by one ROM body. There is
//      something to bind. The 0x854-byte band is live mutable state written
//      through the accessors and read through data_020a9570 by
//      src/func_02065af0.c in the same session; a publish-at-load copy would
//      need a copy-back at every access and the ROM offers no point to do it.
//   2. MSVC WILL NOT PLACE A C SYMBOL AT AN ABSOLUTE ADDRESS, and ntr/io.cpp
//      already wrote down the experiment: a bss_seg plus
//      /BASE /FIXED /DYNAMICBASE:NO does pin 0x02000000, and it was rejected
//      because /FIXED turns a lost range into a process that never starts with
//      no message of ours. data_020a9570 is read by relocation as
//      &data_020a9570 from src/func_02065af0.c, so it has to stay a host
//      symbol.
//   3. DS MAIN RAM IS THE ONE RANGE THE PORT SHIPS ABLE TO LOSE. ntr/io.cpp's
//      region table marks main memory fatal:false, on the stated ground that
//      "in the port build the game's globals are host-linked symbols". Putting
//      a session's own state there makes a survivable loss fatal.
//   4. NOTHING AT 0x020A94D4 IS IN A SAVE STATE OR A ROLLBACK FRAME.
//      hal/lk6_savestate.cpp captures the arena plus [dsstate_lo, dsstate_hi);
//      hal/rollback.cpp hashes and restores the same two plus the hardware
//      log. A band in DS main RAM is in neither, and this band is on the VS
//      determinism path, which is the one place that would be found the
//      hard way.
//
// THE SEVEN BODIES BEHIND THE TWO GUARDS ARE LINKED AND UNENTERED, and that is
// the honest state rather than a defect: src/func_02065050.c and
// src/func_020650d8.c are reached only from src/func_020417d0.c's message
// types 0x15 and 0x9, and func_020417d0 is entered only when the host ARM7
// posts an 0x82 port-receive indication, which it does not. The [wm8] census
// at the bottom of this file reads that back out of the ROM's own bytes.
//
// ============================ THE STORAGE ==================================
//
// ONE CONTIGUOUS BAND, 0x2b80 BYTES, AND IT HAS TO BE ONE. src/func_02040c34.c
// derives its whole working area from data_020a1fc0:
//
//     02040C4C  ldr sl, [pc, #0x2b0]     sl = data_020a1fc0
//     02040CF8  add fp, sl, #0x440       fp = the pool base
//     02040D0C  add sb, fp, #0xc0        the first 0xcc0-byte node
//     02040D18  add r8, fp, #0x2700      the end of the three nodes
//
// while src/func_020412f0.c and src/func_02042254.c spell the same base as
// `data_020a2400`. data_020a1fc0 + 0x440 IS data_020a2400 on the DS, so the two
// spellings have to be the same address here, and the third node
// (fp + 0x1a40 .. fp + 0x2700) runs straight through data_020a3fc0. The band is
// therefore data_020a1fc0, data_020a2400, data_020a2404, data_020a2408,
// data_020a2409, data_020a3fc0, in that order, with no padding:
//
//     +0x0000  data_020a1fc0  0x440   the control block  (hal/comms_conductor.cpp,
//                                     .dsstate$ymp3s0009, NOT MOVED)
//     +0x0440  data_020a2400  4       the pool base
//     +0x0444  data_020a2404  4
//     +0x0448  data_020a2408  1
//     +0x0449  data_020a2409  0x1bb7
//     +0x2000  data_020a3fc0  0xb80   MOVED HERE from hal/comms_seam.cpp
//     +0x2b80                         = 0x020a4b40, the next ROM symbol
//
// WHY THE SUFFIXES ARE $ymp3s0010..0014 AND NOT A TAIL APPEND. The band's head
// is data_020a1fc0 and it already lives at ".dsstate$ymp3s0009" in
// hal/comms_conductor.cpp, which this lane does not own and does not touch. A
// section named ".dsstate$ymp3s0010" sorts immediately after it (tenth
// character, "0" < "1" at the ninth) and before ".dsstate$ymp3t0000", so the
// five members below land right behind the head with nothing between them.
// Every member is align(1) exactly so that no padding can be inserted: an
// aligned member would break the contiguity the ROM's arithmetic depends on,
// silently, and the check at the bottom of this file exists because "silently"
// is not acceptable for 0x2b80 bytes of write.
//
// WHAT MOVES, MEASURED RATHER THAN HOPED. This is NOT a pure tail append and
// saying so is the point. Three edits to the .dsstate layout net out:
//     + 0x2740 inserted at $ymp3s0010..0014 (this file)
//     - 0x0b80 removed at $ywm             (data_020a3fc0 came here)
//     - 0x1bc0 removed at $ywmd13          (data_020a2400 came here)
// so every hosted global from $ywmd14 onward is at exactly the byte it was at
// before, and the ones between -- $ymp3t0000..0002 (+0x2740) and
// $ywmb / $ywmc00..18 / $ywmd00..12 (+0x1bc0) -- move. All of them are lanes
// WM1/WM2/WM3's wireless state and the MP3 conductor's; none is geometry, and
// none reaches a display list as a pointer value. The battery's BMP rows are
// what checks that claim rather than this paragraph.
//
// FOUR MORE NAMES, TAIL-APPENDED AT $ywmf, where nothing moves:
//     data_020a0f88  4      bss   func_02040c34's second once-word
//     data_020a0f8c  4      bss   func_020412f0's 0x3c-frame counter
//     data_02082158  0x20   data  the block func_020658c0 copies into a node
//     data_02099e24  0x48   data  and this one is worth reading: it is the
//                                 string "WFSi_ForceWakeupCard() called!
//                                 (unknown bug has been force-recovered)\n",
//                                 which names the whole subsystem. This chain
//                                 is NitroSDK's WFS -- the Wireless File
//                                 System, which streams the cartridge's own
//                                 files to a child DS -- and that is why a
//                                 body whose only WM touch is one callback
//                                 registration drags in eleven CARD_FS rows.
// Both data rows were read out of extracted/arm9_dec.bin at their addresses
// (config/arm9/relocs.txt has no relocation inside either span, so the bytes
// are literal), not assumed to be zero.
//
// AND ONE SPLIT IN hal/wm_arm7.cpp, address-neutral: its ".dsstate$ywmd12"
// hosted data_020a11e4 at 1180 = 0x400 + 0x9c, deliberately covering
// data_020a15e4's span as well because "nothing names data_020a15e4". This
// rung names it -- it is the OSThread record func_02042200 passes to
// func_02058200 and func_020423c8 wakes -- so the one object becomes two
// sections, $ywmd12a (0x400) and $ywmd12b (0x9c), same total, same order, same
// first byte.
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <stdint.h>

extern "C" {

// The band's head. Defined by hal/comms_conductor.cpp at ".dsstate$ymp3s0009";
// this file only reads its address, to check that the five members below
// really landed behind it.
extern unsigned char data_020a1fc0[];

// The OSThread record and its 1 KB stack, both in hal/wm_arm7.cpp.
extern unsigned char data_020a11e4[];
extern unsigned char data_020a15e4[];

// The 0x02063E-0x020655 message layer's own 0x854-byte object, in two
// hal/wm_arm7.cpp sections: data_020a94d4 is the 0x9c header that
// src/func_02065b94.c's literal names, data_020a9570 the 0x7b8 tail that
// src/func_02065b88.c's names. The band check below reads the 0x9c back and
// the [wm8] census reads the ROM's own state out of them.
extern unsigned char data_020a94d4[];
extern unsigned char data_020a9570[];
// The layer's up flag: 0 until src/func_020652fc.c or src/func_02065234.c
// sets it, and nothing else in the image writes it.
extern int data_020a94c0;

// The ROM's sixteen-slot thread table (hal/cxx_aliases.cpp) and the ROM bodies
// this file's census reads back through. Every one is already linked.
extern int data_020a6148[16];

// The session's role byte, hosted by hal/comms_conductor.cpp: 1 parent, 2 child,
// 0 no session. The census reads it so that a zero worker count is reported as
// the reading it is rather than as a miss -- src/func_02040c34.c only reaches
// func_02042200 on the role-1 arm, so a child and a solo run are RIGHT to have
// no worker, and only a parent with no worker is a failure.
extern unsigned char data_020a0f04[];
void func_02042254(void);
void func_020581a8(void);
void func_02058048(void *record);
void port_thread_sched_counts(unsigned long long *adopted,
                              unsigned long long *entries,
                              unsigned long long *switches,
                              unsigned long long *rejected,
                              unsigned long long *nocreate);
// Rung W7's own census, at the bottom of this file.
void port_wm8_band_report(void);

// ---------------------------------------------------------------------------
// THE BAND. align(1) on every member is load-bearing: see the header.
// ---------------------------------------------------------------------------
#define WM5_BAND(sec, name, size)                                 \
    __pragma(section(sec, read, write))                           \
    extern "C" __declspec(allocate(sec)) __declspec(align(1))     \
    unsigned char name[size]

/* +0x0440  the pool base. src/func_020412f0.c and src/func_02042254.c spell
   the base with this name; src/func_02040c34.c spells it data_020a1fc0+0x440. */
WM5_BAND(".dsstate$ymp3s0010", data_020a2400, 4) = { 0 };
/* +0x0444  src/func_020412f0.c writes a word here per message slot. */
WM5_BAND(".dsstate$ymp3s0011", data_020a2404, 4) = { 0 };
/* +0x0448  one byte, and src/func_020412f0.c stores a byte to it by name. */
WM5_BAND(".dsstate$ymp3s0012", data_020a2408, 1) = { 0 };
/* +0x0449  0x1bb7 to the next ROM symbol. NOT 4-aligned on the DS either --
   the ROM symbol really is at an odd address, which is why the whole run is
   align(1). src/func_020412f0.c's byte-triple store is the reader. */
WM5_BAND(".dsstate$ymp3s0013", data_020a2409, 0x1bb7) = { 0 };
/* +0x2000  0xb80 to data_020a4b40. MOVED HERE from hal/comms_seam.cpp's
   ".dsstate$ywm". That file hosted it standing alone on the ground that a
   relocation sweep of config/arm9/relocs.txt over its span returns exactly two
   ROM functions, func_02040a5c and func_02040a84. The sweep is correct and the
   conclusion was not: src/func_02040c34.c reaches these bytes by POINTER
   ARITHMETIC off data_020a1fc0, which a relocation sweep cannot see, and its
   third work node (data_020a1fc0 + 0x1e80 .. + 0x2b40) covers most of them.
   The two accessors' own invariant is untouched by the move -- they read and
   write one word of this object and take its address by relocation, so where
   the object sits is not something they can observe. */
WM5_BAND(".dsstate$ymp3s0014", data_020a3fc0, 0xb80) = { 0 };

// ---------------------------------------------------------------------------
// THE FOUR TAIL-APPENDED NAMES. ".dsstate$ywmf00" and up sort after lane WM4's
// ".dsstate$ywme00" (fifth character, "e" < "f") and before the "$zzz" high
// sentinel, so these four grow the captured span at its tail and move nothing.
// ---------------------------------------------------------------------------
#define WM5_TAIL(sec, name, size)                                 \
    __pragma(section(sec, read, write))                           \
    extern "C" __declspec(allocate(sec)) __declspec(align(4))     \
    unsigned char name[size]

/* 0x020a0f88, 4, bss. src/func_02040c34.c:57 clears it. */
WM5_TAIL(".dsstate$ywmf00", data_020a0f88, 4) = { 0 };
/* 0x020a0f8c, 4, bss. src/func_020412f0.c's case-0xd counter: sixty frames
   without a served read and it calls func_0204175c, the force-wakeup. */
WM5_TAIL(".dsstate$ywmf01", data_020a0f8c, 4) = { 0 };
/* 0x02082158, 0x20, kind:data. src/func_020658c0.c does
   CpuCopy8(a2, node + 8, 0x20) with this as a2. Thirty-two zero bytes in the
   cartridge image; read out rather than assumed. */
WM5_TAIL(".dsstate$ywmf02", data_02082158, 0x20) = { 0 };
/* 0x02099e24, 0x48, kind:data. The WFS force-wakeup message string; see the
   header. src/func_0204175c.c passes its address to the game-card handler at
   data_0209a03c, which config/arm9/relocs.txt resolves to func_02057e30. */
WM5_TAIL(".dsstate$ywmf03", data_02099e24, 0x48) = {
    0x57, 0x46, 0x53, 0x69, 0x5f, 0x46, 0x6f, 0x72, 0x63, 0x65, 0x57, 0x61,
    0x6b, 0x65, 0x75, 0x70, 0x43, 0x61, 0x72, 0x64, 0x28, 0x29, 0x20, 0x63,
    0x61, 0x6c, 0x6c, 0x65, 0x64, 0x21, 0x20, 0x28, 0x75, 0x6e, 0x6b, 0x6e,
    0x6f, 0x77, 0x6e, 0x20, 0x62, 0x75, 0x67, 0x20, 0x68, 0x61, 0x73, 0x20,
    0x62, 0x65, 0x65, 0x6e, 0x20, 0x66, 0x6f, 0x72, 0x63, 0x65, 0x2d, 0x72,
    0x65, 0x63, 0x6f, 0x76, 0x65, 0x72, 0x65, 0x64, 0x29, 0x0a, 0x00, 0x00
};

#undef WM5_BAND
#undef WM5_TAIL

// ---------------------------------------------------------------------------
// THE FOUR FACES INTO THE 0x02063E-0x020655 TABLE BAND ARE RETIRED (rung W7).
//
// func_020652fc, func_02065234, func_02065050 and func_020650d8 stood here as
// counting no-ops. All four are ROM bodies on port/slice_wm8.txt now, with the
// seven behind them, and the counters below stay so that the census line keeps
// its shape and reads 0/0/0/0 for the rest of this port's life. A nonzero
// number in that field would mean a face came back.
//
// EACH REFUSAL, AND THE ANSWER IT GOT:
//   func_020652fc  refused as "the parent's message-layer init ... leaves
//                  func_020412f0 uninstalled as the callback". ANSWERED: it is
//                  the ROM's own body, it sets data_020a94c0 = 1, initialises
//                  the sixteen 0x68-byte entries and stores the callback.
//   func_02065234  refused as "the child's copy of the same". ANSWERED the
//                  same way, on the child's arm.
//   func_02065050  refused as one of "the two guards src/func_020417d0.c
//   func_020650d8  calls ... both forward into the band". ANSWERED: both
//                  forward into func_020654d0 / func_02065500 -> func_02063ea8
//                  for real. They remain UNENTERED, for the reason the refusal
//                  itself gave and which rung W7 does not change: the host
//                  ARM7 posts no 0x82 port-receive indication, so
//                  func_020417d0 is linked-and-unentered and so are they.
//   the band's address  refused as "two ROM accessors that return LITERAL DS
//                  ADDRESSES ... one ROM object as two ... reaches 0x020A9D83".
//                  ANSWERED at the top of this file: the two decimals were
//                  misread, both accessors name a hosted symbol through
//                  config/arm9/relocs.txt, the object is 0x854 bytes at
//                  0x020A94D4, the walk stops at 0x020A9CC3, and the two calls
//                  are renamed per source onto the host addresses.
// ---------------------------------------------------------------------------

static unsigned long g_wm5_face_hits[4];

}  // extern "C"

// ---------------------------------------------------------------------------
// THE BAND CHECK. hal/globals_link100.cpp's port_link100_card_check is the
// worked example; this one refuses rather than only reporting, because a band
// that came apart is 0x2b80 bytes of writes landing on whatever the linker put
// next, and the first symptom would be an unrelated global changing under a
// session.
// ---------------------------------------------------------------------------
namespace {

// These symbols are separate arrays. Compare their integer addresses; linker
// spacing does not make them elements of one C++ array.
unsigned long address_offset(const void *address, const void *base)
{
    return static_cast<unsigned long>(reinterpret_cast<uintptr_t>(address) -
                                      reinterpret_cast<uintptr_t>(base));
}

struct BandRow { const unsigned char *p; unsigned long want; const char *name; };

int wm5_band_check(void)
{
    const BandRow k[] = {
        { data_020a1fc0, 0x0000, "data_020a1fc0" },
        { data_020a2400, 0x0440, "data_020a2400" },
        { data_020a2404, 0x0444, "data_020a2404" },
        { data_020a2408, 0x0448, "data_020a2408" },
        { data_020a2409, 0x0449, "data_020a2409" },
        { data_020a3fc0, 0x2000, "data_020a3fc0" },
    };
    int bad = 0;
    for (int i = 0; i < 6; ++i) {
        const unsigned long got = address_offset(k[i].p, data_020a1fc0);
        if (got != k[i].want) {
            std::fprintf(stderr,
                         "  [wm5] BAND BROKEN: %s at +0x%lx, the ROM says "
                         "+0x%lx\n", k[i].name, got, k[i].want);
            bad = 1;
        }
    }
    // And the OSThread record's own offset inside hal/wm_arm7.cpp's split.
    if (address_offset(data_020a15e4, data_020a11e4) != 0x400) {
        std::fprintf(stderr,
                     "  [wm5] BAND BROKEN: data_020a15e4 at +0x%lx of "
                     "data_020a11e4, the ROM says +0x400\n",
                     address_offset(data_020a15e4, data_020a11e4));
        bad = 1;
    }
    // AND THE SECOND GROUPED RUN, rung W7's (run link100, lane WM8): the
    // 0x02063E-0x020655 layer's 0x854-byte object, hosted as data_020a94d4
    // (".dsstate$ywmd18a", 0x9c) followed by data_020a9570 ("$ywmd19", 0x7b8).
    // src/func_020652fc.c's sixteen-iteration loop walks from the first and
    // its table runs to the end of the second, so a gap between them is the
    // same class of defect the six rows above check for.
    if (address_offset(data_020a9570, data_020a94d4) != 0x9c) {
        std::fprintf(stderr,
                     "  [wm8] BAND BROKEN: data_020a9570 at +0x%lx of "
                     "data_020a94d4, the ROM says +0x9c\n",
                     address_offset(data_020a9570, data_020a94d4));
        bad = 1;
    }
    if (bad) {
        std::fprintf(stderr,
                     "  [wm5] the wireless work area is not contiguous; "
                     "src/func_02040c34.c would write 0x2b80 bytes across "
                     "whatever the linker put next. Refusing to run.\n");
        std::fflush(stderr);
        std::exit(3);
    }
    return bad;
}

struct Wm5BandCheck { Wm5BandCheck() { wm5_band_check(); } };
Wm5BandCheck g_wm5_band_check;

// ---------------------------------------------------------------------------
// THE CENSUS. Same shape as hal/thread_create.cpp's rungs, on the ROM's own
// bytes, for the SECOND created thread. It is a one-shot driven from
// hal/wm_arm7.cpp's port_arm7_wireless_tick (the host ARM7's turn, which runs
// at the ARM9's yield points), because that is the first owned code that runs
// after src/func_02042200.c has created the thread and the ROM's own
// reschedule has entered it.
//
// Rungs, in the order thread_create_proof.py's own verdict names them:
//   created   the record data_020a15e4 carries exactly what src/func_02058200.c
//             writes for THIS thread -- entry pc + 4 = func_02042254 + 4, the
//             exit thunk &func_020581a8 at +0x3c, the stack span
//             [data_020a11e4, data_020a11e4 + 0x400) with the two guard words
//             at both ends, and data_020a6148[id] pointing back at the record.
//   entered   hal/boot2_thread.cpp adopted it and the fiber ran: measured as a
//             rise in the scheduler's adoption and entry counters across the
//             creation, plus the ROM's own state word.
//   slept     the thread's state word (+0x64) is 0 again, which is what
//             src/OS_SleepThread.c leaves after src/func_02042254.c's
//             `OS_SleepThread(0)` at 0x020422D0.
//   woken     func_02058048(data_020a15e4) -- src/func_020423c8.c's whole body,
//             the ROM's own wake for this thread -- puts it back on the
//             scheduler and the reschedule switches to it. Ends where it
//             started, parked, because nothing has pushed work.
// ---------------------------------------------------------------------------

uint32_t rec32(unsigned off)
{
    return *reinterpret_cast<const uint32_t *>(data_020a15e4 + off);
}

uint32_t at32(uint32_t a)
{
    return *reinterpret_cast<const uint32_t *>(static_cast<uintptr_t>(a));
}

uint32_t addr_of(const void *p)
{
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p));
}

uint32_t fn_addr(void (*f)(void))
{
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(f));
}

const uint32_t kGuardBase = 0x7bf9dd5bu;   // src/func_02058200.c, str r2,[r1]
const uint32_t kGuardTop  = 0xfddb597du;   // src/func_02058200.c, str r1,[r0,#-4]
const uint32_t kStack     = 0x400u;        // src/func_02042200.c's 5th argument
const uint32_t kPrio      = 0x0fu;         // src/func_02042200.c's 6th argument

int g_census_done;
unsigned long long g_ad0, g_en0, g_sw0, g_rj0, g_nc0;
int g_baseline_taken;

}  // namespace

extern "C" void port_wm5_worker_census(void)
{
    if (g_census_done) return;

    if (!g_baseline_taken) {
        port_thread_sched_counts(&g_ad0, &g_en0, &g_sw0, &g_rj0, &g_nc0);
        g_baseline_taken = 1;
    }

    // Nothing to census until src/func_02042200.c has run. Its own signature
    // in the record is the entry pc, and it is zero until then.
    const uint32_t pc4 = rec32(0x40);
    if (pc4 == 0) return;

    g_census_done = 1;

    const uint32_t id   = rec32(0x6c);
    const uint32_t prio = rec32(0x70);
    const uint32_t lo   = rec32(0x84);
    const uint32_t hi   = rec32(0x88);
    const uint32_t lr   = rec32(0x3c);
    const uint32_t base = addr_of(data_020a11e4);
    const uint32_t want_pc4 = fn_addr(&func_02042254) + 4;
    const uint32_t want_lr  = fn_addr(&func_020581a8);

    const bool slot_ok = id < 16 &&
        data_020a6148[id] == static_cast<int>(addr_of(data_020a15e4));
    const bool span_ok = lo == base && hi == base + kStack;
    const bool guard_ok = span_ok && at32(lo) == kGuardBase &&
                          at32(hi - 4) == kGuardTop;
    const bool ctx_ok = pc4 == want_pc4 && lr == want_lr && prio == kPrio;
    const bool created_ok = slot_ok && span_ok && guard_ok && ctx_ok;

    unsigned long long ad1, en1, sw1, rj1, nc1;
    port_thread_sched_counts(&ad1, &en1, &sw1, &rj1, &nc1);
    const uint32_t st_before = rec32(0x64);
    const bool entered_ok = (en1 - g_en0) >= 1 && (ad1 - g_ad0) >= 1;
    const bool slept_ok = entered_ok && st_before == 0;

    std::fprintf(stderr,
                 "[wm5] CREATED by func_02042200 (the ROM's own): id=%u "
                 "prio=%u stack=[%08x,%08x) guards=%08x/%08x "
                 "pc4=%08x(want %08x) lr=%08x(want %08x) slot=%08x rung1=%s\n",
                 id, prio, lo, hi,
                 span_ok ? at32(lo) : 0u, span_ok ? at32(hi - 4) : 0u,
                 pc4, want_pc4, lr, want_lr,
                 id < 16 ? (unsigned)data_020a6148[id] : 0u,
                 created_ok ? "OK" : "FAIL");
    std::fflush(stderr);

    // Rung 4: the ROM's own wake for this thread, which is the whole body of
    // src/func_020423c8.c. Called here rather than transcribed.
    func_02058048(data_020a15e4);

    unsigned long long ad2, en2, sw2, rj2, nc2;
    port_thread_sched_counts(&ad2, &en2, &sw2, &rj2, &nc2);
    const unsigned long long switches = sw2 - sw1;
    const uint32_t st_after = rec32(0x64);
    const bool woken_ok = switches >= 2 && st_after == 0;

    const bool all = created_ok && entered_ok && slept_ok && woken_ok;
    std::fprintf(stderr,
                 "[wm5] PROOF created=%d entered=%d slept=%d woken=%d "
                 "adopted=%llu entries=%llu switches=%llu rejected=%llu "
                 "nocreate=%llu state_before=%u state_after=%u "
                 "faces=%lu/%lu/%lu/%lu VERDICT %s\n",
                 (int)created_ok, (int)entered_ok, (int)slept_ok,
                 (int)woken_ok, ad2 - g_ad0, en2 - g_en0, switches, rj2, nc2,
                 st_before, st_after,
                 g_wm5_face_hits[0], g_wm5_face_hits[1],
                 g_wm5_face_hits[2], g_wm5_face_hits[3],
                 all ? "WORKER-RAN" : (created_ok ? "NOT-ENTERED"
                                                  : "NOT-CREATED"));
    std::fflush(stderr);
}

// The solo census. Printed once at exit whatever happened, so a solo run can
// state the negative -- no thread record, no face hit -- rather than being
// silent about it. hal/wm_arm7.cpp calls this from its own exit report.
extern "C" void port_wm5_report(void)
{
    const unsigned role = data_020a0f04[0];
    const int created = (int)(rec32(0x40) != 0);
    // What the number MEANS on this window, spelled out next to it. Only the
    // parent's arm of src/func_02040c34.c reaches func_02042200; the child's
    // arm ends at func_02065234, and a solo run never enters func_02040c34 at
    // all. So worker_created=0 is the CORRECT reading on a child and on a solo
    // run, and a miss only on a parent.
    const char *arm =
        role == 1 ? (created ? "parent: the ROM's role-1 arm created it, as it must"
                             : "parent: MISS, the role-1 arm should have created one")
      : role == 2 ? (created ? "child: UNEXPECTED, the ROM's role-0 arm creates no worker"
                             : "child: correct, the ROM's role-0 arm creates no worker")
      :             (created ? "solo: UNEXPECTED, no session, so func_02040c34 is never called"
                             : "solo: correct, no session, so func_02040c34 is never called");
    std::fprintf(stderr,
                 "[wm5] census: worker_created=%d record_pc4=%08x "
                 "faces=%lu/%lu/%lu/%lu band_base=%p band_len=0x%lx "
                 "role=%u (%s)\n",
                 created, rec32(0x40),
                 g_wm5_face_hits[0], g_wm5_face_hits[1],
                 g_wm5_face_hits[2], g_wm5_face_hits[3],
                 (const void *)data_020a1fc0,
                 (unsigned long)((data_020a3fc0 + 0xb80) - data_020a1fc0),
                 role, arm);
    std::fflush(stderr);
    port_wm8_band_report();
}

// ---------------------------------------------------------------------------
// THE [wm8] CENSUS: DID THE ELEVEN BODIES RUN, READ OUT OF THE ROM'S OWN BYTES.
//
// There is no host counter in any of the eleven and there must not be: they are
// matched ROM bodies and nothing in this port may edit them. So the census
// reads back the state each one is the only writer of, which is the same shape
// hal/wm_thread.cpp's worker census already uses on the OSThread record and a
// stronger instrument than a wrapper would be -- a wrapper counts calls, this
// counts the ROM's own effects.
//
// Layout of the object, all offsets from data_020a94d4 (= 0x020A94D4):
//     +0x000   the layer's own state word (src/func_02063ea8.c reads it)
//     +0x008   the peer bitmap (u16)
//     +0x148   the argument src/func_020652fc.c / src/func_02065234.c store
//     +0x1d4   sixteen 0x68-byte entries, running to +0x854 = 0x020A9D28
//   entry i:  +0x00  src/func_02063ea8.c writes 1 on entry and 2 when it banks
//             +0x03  the byte src/func_020652fc.c's sixteen-iteration loop
//                    clears (p + 0x1d7 with p = base + i * 0x68)
//             +0x24  the sub-record src/func_020647a4.c / src/func_020647d8.c
//                    walk, whose +0x10 is func_020647a4's own received counter
//
// WHAT EACH FIELD PROVES:
//   up            data_020a94c0 == 1. Only src/func_020652fc.c and
//                 src/func_02065234.c write it, and both were faced until
//                 rung W7, so a 1 here IS the entry of one of them.
//   arg148        the value that same body stored, which says which arm ran:
//                 func_020652fc stores its third argument, func_02065234 the
//                 only one it has.
//   started/banked  how many of the sixteen entries carry src/func_02063ea8.c's
//                 own 1 or 2 at +0x00 -- its entry count, and with it the entry
//                 count of src/func_020654d0.c, src/func_02065500.c and
//                 everything they reach.
//   received      the sum of the sixteen +0x24+0x10 words, which only
//                 src/func_020647a4.c increments.
// ---------------------------------------------------------------------------
// RUNG W8, EXTENDED BY LANE WM9: the fan-out's own writer counts, so the
// receive census and the ownership claim are read on ONE line. Defined in
// hal/comms_fanout_wide.cpp, which goes to every target this file goes to and
// two more (SLICE_COMMS_SOURCES plus mp_comms_seam), so this reference cannot
// strand a probe the way a reference the other way round would.
extern "C" void port_wm9_fanout_writers(unsigned long long *frames,
                                        unsigned long long *touch,
                                        unsigned long long *pad,
                                        unsigned long long *foreign);

// RUNG W9, LANE WM10: the third line of the same question. Defined in
// hal/wm_arm7.cpp, which goes wherever this file goes.
extern "C" void port_wm10_indication_report(void);

extern "C" void port_wm8_band_report(void)
{
    const unsigned char *base = data_020a94d4;
    const unsigned long span = address_offset(data_020a9570 + 1976, data_020a94d4);
    int started = 0, banked = 0;
    unsigned long received = 0;

    for (int i = 0; i < 16; ++i) {
        const unsigned char *e = data_020a9570 + (0x1d4 - 0x9c) + i * 0x68;
        if (e[0] == 1) ++started;
        if (e[0] == 2) ++banked;
        received += *reinterpret_cast<const uint32_t *>(e + 0x24 + 0x10);
    }

    const int up = (data_020a94c0 == 1);
    const char *why =
        up ? (started || banked
                 ? "the layer is up and src/func_02063ea8.c has run"
                 : "the layer is up; the seven behind the two guards are "
                   "LINKED AND UNENTERED, because src/func_020417d0.c needs an "
                   "0x82 port-receive indication and the host ARM7 posts none")
           : "the layer was never brought up: no session on this window, so "
             "src/func_02040c34.c never reached either initialiser";

    unsigned long long wf_frames = 0, wf_touch = 0, wf_pad = 0, wf_foreign = 0;
    port_wm9_fanout_writers(&wf_frames, &wf_touch, &wf_pad, &wf_foreign);

    std::fprintf(stderr,
                 "[wm8] band: base=%p span=0x%lx (ROM 0x854) up=%d "
                 "arg148=%08x entries_started=%d entries_banked=%d "
                 "received=%lu (%s)\n",
                 (const void *)base, span, up,
                 *reinterpret_cast<const uint32_t *>(data_020a9570 + (0x148 - 0x9c)),
                 started, banked, received, why);
    // LANE WM9: THE OTHER HALF OF THE SAME QUESTION. The line above says
    // whether the ROM's own receive path ran; this one says whether anything
    // but the fan-out wrote the game's input words while it did. touch and pad
    // are the two dispatchers' call counts and equal `frames` on a correct
    // run; `foreign` counts frames where one of the five fanned-out regions
    // changed between the end of step 0x17 and the head of the next step 0x16,
    // which is the only window in the ROM's loop where a SECOND writer could
    // do it unseen. Anything but zero there is a datagram landing twice.
    std::fprintf(stderr,
                 "[wm9] fan-out writers: frames=%llu touch=%llu pad=%llu "
                 "foreign=%llu (%s)\n",
                 wf_frames, wf_touch, wf_pad, wf_foreign,
                 wf_foreign ? "A SECOND WRITER TOUCHED THE GAME WORDS"
                            : "the fan-out is the only writer of the game "
                              "words, once a frame");
    std::fflush(stderr);
    // LANE WM10: and whether the port-0x0c port-receive indication reached the
    // ROM's own MP receive callbacks, read out of the cartridge's own words.
    port_wm10_indication_report();
}
