/* HOST COPY of src/_ZN4Coin8BehaviorEv.cpp -- the coin's behaviour-state
 * dispatch read as a plain function pointer, and the table seated with host
 * addresses.
 *
 * Coin is one class serving three actor ids (288 Coin, 289 RedCoin, 290
 * BlueCoin) and nine behaviour states, and Behavior picks the state it is in
 * by indexing data_ov002_0210dc70 with mBehaviorType (+0x3a4).
 * data_ov002_0210dc70 is ov002 BSS, 0x48 bytes -- nine mwcc POINTER-TO-MEMBER
 * pairs -- and __sinit_ov002_02100c50 fills it out of the nine statics at
 * ov002 0x02108730..0x02108770. Every one is the plain nonvirtual
 * { function, 0 } form, so the host reads the first word back as a function
 * taking `this` and ignores the second: the OneUpMushroom's treatment
 * (port/unmatched/OneUpMushroom_Behavior.cpp), which carries the long version
 * of why MSVC cannot be handed the source line as written.
 *
 * THE SEAT IS THE SAME TWO-PART CHECK. The sinit really does run -- it is
 * matched src and it is on the gate-10 boot list -- but what it copies are
 * the ov002 image's own words, which are DS CODE ADDRESSES. The seat below
 * rewrites each pair's first word with the host body after checking that the
 * word the sinit left is the ROM address that body was compiled from, so a
 * mount pointing at the wrong bytes says so instead of calling into the
 * overlay image.
 *
 * TWO OF THE NINE ENTRIES ARE DUPLICATES and that is the ROM's own table, not
 * a transcription slip: states 2 and 6 are both ov002 0x020b1cc0, and states
 * 1 and 7 are both 0x020b20b4. Seven distinct bodies, nine slots.
 *
 * The body below is the matched source's control flow line for line: the
 * pickup-sound flag, the blue-coin area retire, the two early outs, the spin,
 * the state call, and the two cylinder clear/update arms.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned id, void *pos);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
int LenVec3(void *v);
extern unsigned char data_0209f2d8;      /* the mega-char state byte */

int func_ov002_020b10a0(void *self);     /* "am I collected / gone" */
int func_ov002_020b12ec(void *self);     /* "am I in the retire path" */
void func_ov002_020b14d8(void *self);
void func_ov002_020b10e4(void *self);
int func_ov002_020b19dc(void *self);

/* the nine states, in the order __sinit_ov002_02100c50 seats them */
void func_ov002_020b2150(void *);
void func_ov002_020b20b4(void *);
void func_ov002_020b1cc0(void *);
void func_ov002_020b1bfc(void *);
void func_ov002_020b2070(void *);
void func_ov002_020b1ad4(void *);
void func_ov002_020b1a60(void *);

struct PortPmf { unsigned fn; int delta; };
extern PortPmf data_ov002_0210dc70[];

}  /* extern "C" */

enum { PORT_COIN_STATES = 9 };

/* ---- THE SEVEN FACES, AND THE ALIAS (run link100, lane PMFB2) -----------
   src/_ZN4Coin8BehaviorEv.cpp is a real pointer-to-member dispatch and MSVC
   emits it as

       mov ecx, <table>[eax*8+4]     the adjust word
       mov eax, <table>[eax*8]       the code word
       add ecx, <this>
       call eax
   -- `call <reg>` with the receiver in ECX and nothing pushed. The state
   bodies are plain cdecl and read the receiver off the stack, so the code word
   holds a __fastcall FACE and the face calls the body. Nine slots, seven distinct bodies (the
   ROM's own table repeats two of them); each distinct body gets one face.
   Nothing else in the port reads data_ov002_0210dc70.

   THE ALIAS. The matched TU declares `extern PMF data_ov002_0210dc70[];`
   OUTSIDE its extern "C" block, so it comes in as
   ?data_ov002_0210dc70@@3PAP8C@@AEXXZA -- the name a listing of the TU under
   the port's own flags shows. The ov002 mount defines the storage as the plain
   C name, so the slice needs this one directive to link, and the directive
   lives here because this file is the seat that owns the table
   (the hal/bob_enemy_bridges.cpp precedent). alternatename_guard's rule holds:
   the LHS is never DEFINED anywhere, only referenced. */
#pragma comment(linker, "/alternatename:?data_ov002_0210dc70@@3PAP8C@@AEXXZA=_data_ov002_0210dc70")

#define COIN_FACE(sym)                                                        \
    static void __fastcall pmf_face_##sym(void *self, void *dead_edx)         \
    { sym(self); }
COIN_FACE(func_ov002_020b2150)
COIN_FACE(func_ov002_020b20b4)
COIN_FACE(func_ov002_020b1cc0)
COIN_FACE(func_ov002_020b1bfc)
COIN_FACE(func_ov002_020b2070)
COIN_FACE(func_ov002_020b1ad4)
COIN_FACE(func_ov002_020b1a60)
#undef COIN_FACE

#define CF(sym) (void (*)(void *))pmf_face_##sym
static const struct { unsigned rom; void (*host)(void *); } g_states[] = {
    {0x020b2150, CF(func_ov002_020b2150)},   /* 0 <- 0x02108730 */
    {0x020b20b4, CF(func_ov002_020b20b4)},   /* 1 <- 0x02108770 */
    {0x020b1cc0, CF(func_ov002_020b1cc0)},   /* 2 <- 0x02108748 */
    {0x020b1bfc, CF(func_ov002_020b1bfc)},   /* 3 <- 0x02108760 */
    {0x020b2070, CF(func_ov002_020b2070)},   /* 4 <- 0x02108740 */
    {0x020b1ad4, CF(func_ov002_020b1ad4)},   /* 5 <- 0x02108758 */
    {0x020b1cc0, CF(func_ov002_020b1cc0)},   /* 6 <- 0x02108750 (same as 2) */
    {0x020b20b4, CF(func_ov002_020b20b4)},   /* 7 <- 0x02108768 (same as 1) */
    {0x020b1a60, CF(func_ov002_020b1a60)},   /* 8 <- 0x02108738 */
};
#undef CF

extern "C" void port_coin_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < PORT_COIN_STATES; ++i) {
        if (data_ov002_0210dc70[i].fn != g_states[i].rom) {
            std::fprintf(stderr, "FATAL: coin state %d: the sinit left %08x, "
                         "the ROM's own table says %08x -- WRONG BYTES\n", i,
                         data_ov002_0210dc70[i].fn, g_states[i].rom);
            std::abort();
        }
        data_ov002_0210dc70[i].fn = (unsigned)(size_t)g_states[i].host;
        data_ov002_0210dc70[i].delta = 0;
    }
}

/* _ZN4Coin8BehaviorEv RETIRED (run link100, lane PMFB2). It is on
   port/slice_pmfb2.txt and compiles from src/_ZN4Coin8BehaviorEv.cpp, which
   recovered as a real C++ method (?Behavior@Coin@@QAEHXZ); the Itanium C name
   its fill site calls is one cdecl line in hal/except_faces.cpp and no fill
   site changes.

   MEASURED FOR THAT LANE. The ROM strides the table by eight
   (`add r3, r1, r0, lsl #3` at 0x020b2410) and the matched TU emits
   [eax*8] -- with and without /Zp4, because its record is the bare
   pointer-to-member and not a struct containing one. All nine source pairs
   (0x02108730..0x02108770) read {code, 0} in
   extracted/overlays/overlay_0002.bin, so `this` is never adjusted and the ROM
   takes its ldreq arm. What was left was the calling convention, and the seven
   faces above are that repair. The header's OTHER reason -- the sinit leaves a
   DS code address in each word -- is unchanged and is still what
   port_coin_states_seat is for. */
