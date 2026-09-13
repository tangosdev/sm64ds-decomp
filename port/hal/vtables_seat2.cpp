// The ROM's own destructor and init words, seated into the host tables that
// stand in for four cartridge vtables.  Run link100 wave 7, lane SEAT2, census
// batch B2 (DTOR-THUNK-VTABLES).
//
// This file is the CTOR3 shape one wave on: a C++ static initialiser that
// fills ROM vtable words from matched member bodies, so the words are live
// before anything -- the ROM's own .ctor walk at Entry included -- can
// dispatch through them.  hal/scene_boot.cpp's l2_seat_fader_vtables is the
// original and its header carries the argument for the mechanism.
//
// ---- WHAT IS SEATED HERE, AND WHERE THE WORDS COME FROM --------------------
//
// Every address below was read out of extracted/arm9_dec.bin at
// (addr - 0x02004000) and resolved against config/arm9/relocs.txt and
// config/arm9/symbols.txt.  Nothing here is copied from a filename.
//
//   _ZTV7Clipper        0x0208e730, TWO function words
//     +0x00  0x02015720  _ZN7ClipperD1Ev          already seated (ctor_runner)
//     +0x04  0x020156fc  _ZN7ClipperD0Ev          SEATED HERE
//     reloc:  from:0x0208e734 kind:load to:0x020156fc
//
//   _ZTV11ShadowModel   0x0208e868, THREE function words
//     +0x00  0x02015ff8  _ZN11ShadowModelD1Ev     already seated (cxxname_bridge)
//     +0x04  0x02015f80  _ZN11ShadowModelD0Ev     SEATED HERE
//     +0x08  0x02015ef4  _ZN11ShadowModel9DoSetFileEPcii  (model_dtor_seat)
//     reloc:  from:0x0208e86c kind:load to:0x02015f80
//
// The other two rows of this batch are seated in the files that already own
// their words, because those words are not host ARRAYS this file can write:
//
//   _ZTV9FaderWipe      0x0208ea9c, +0x04 = 0x02017418 _ZN9FaderWipeD0Ev.
//     That table is MSVC's own ??_7HalFaderWipe@@6B@ with the ROM's symbol
//     aliased onto it (hal/fader_wipes.cpp), so its slot 0x04 is the body of
//     HalFaderWipe::DtorDeleting and the seat is that body calling the matched
//     TU.  A static initialiser cannot write a const vtable.
//   _ZTV5Stage          0x020921c0, +0x04 = 0x0202ddc8
//     _ZN5Stage19BeforeInitResourcesEv.  hal/stage_bridges.cpp's
//     hal_seat_stage_lifecycle() fills all twenty words of that table at boot,
//     AFTER static initialisers run, so a seat from this file would be
//     overwritten and the port's table would still not carry the ROM's word.
//     The seat is in st_binit, the host word definition it replaces.
//
// ---- WHY A STATIC INITIALISER OWNS SLOT 1 OF BOTH TABLES -------------------
//
// Measured rather than assumed, because a second writer would silently undo
// this file: over the whole of port/, the only runtime writers of these two
// tables are hal/cxxname_bridge.cpp's hal_fill_shadow_vtable (word 0) and
// hal/model_dtor_seat.cpp's seat (word 2).  Nothing writes word 1 of either,
// and neither table is ever filled by a loop or a memset.  _ZTV7Clipper's own
// two words are a link-time aggregate initialiser in hal/ctor_runner.cpp, so
// they are in place before any constructor runs and this file's write lands
// after them.  The seat is idempotent and it is the last writer.
//
// ---- WHAT IS NOT SEATED, AND THE MEASUREMENT THAT REFUSES IT ---------------
//
// The batch's other seven rows are refused with evidence, not forgotten.  The
// report and out/SEAT2/ carry the transcripts; the short form:
//
//   _ZN9FaderWipe11AdvanceFadeEv (_ZTV9FaderWipe +0x08).  The matched body
//     renders the wipe mesh -- ModelComponents::Render on this+0x18 -- on
//     every frame of a fade that is neither at 0 nor at 0x1000.  The port does
//     not mount the stage filesystem, so FaderWipe::LoadAndSetFile never runs
//     and that ModelComponents is 0x50 bytes of zero;
//     port/unmatched/ModelComponents_Render.cpp opens on `info->field0` with
//     no null guard.  Seating it is an access violation on the first driven
//     fade, not a link gain.  The host slot keeps HalFaderWipe::AdvanceFade.
//
//   The six _ZThn80_N{9ModelAnim,10ModelAnim2,14BlendModelAnim}D{0,1}Ev rows.
//     Their ROM bodies ARE adjustor thunks and this lane proved the
//     adjustment: each is `ldr r12,[pc,#4]; add r0,r0,r12; b <primary>;
//     .word 0xffffffb0`, a -0x50 this-adjustment ahead of a tail jump into the
//     primary D0/D1, and each thunk table's own offset-to-top word is
//     0xffffffb0 too.  What cannot be seated is the SOURCE: the six matched
//     TUs are mwcc artefact reconstructions of a synthetic two-base hierarchy,
//     and compiled with cl they define ??1ModelAnim@@UAE@XZ (the pair's two
//     files define it identically -- LNK2005) while leaving Base1/Base2's four
//     virtuals permanently undefined (LNK2001), and the only adjustor symbol
//     they emit is a COMDAT ??_E...@@WFA@AEPAXI@Z on an EMPTY shell
//     destructor, not the ROM's teardown.  Pointing a ROM word at that shell
//     would skip the real destructor silently.  out/SEAT2/thunk_msvc_probe.log
//     is the measurement.  The three VTable_Animation_*Thunk tables keep the
//     no-op word hal/cxxname_bridge.cpp and hal/blend_vtable.cpp give them.
//
// ---- THE TRACE KNOB --------------------------------------------------------
//
// SM64DS_SEAT2_TRACE=1 says, once per slot, that a seated word was entered,
// and prints the receiver.  A seat nothing dispatches and a seat that runs on
// every teardown are opposite findings and both are silent otherwise.  Two
// lines a run at most.

#include <cstdio>
#include <cstdlib>

extern "C" {

/* The matched bodies this file seats.  Both are C-named ROM spellings with the
   receiver as an ordinary first argument (src/_ZN7ClipperD0Ev.c and
   src/_ZN11ShadowModelD0Ev.c are both .c). */
void *_ZN7ClipperD0Ev(void *thiz);         /* 0x020156fc */
void *_ZN11ShadowModelD0Ev(void *thiz);    /* 0x02015f80 */

/* The host storage for the two cartridge tables.
   _ZTV7Clipper is hal/ctor_runner.cpp's two-word array; _ZTV11ShadowModel is
   hal/actor_vtables.cpp's eight-word array. */
extern void *_ZTV7Clipper[2];
extern void *_ZTV11ShadowModel[8];

}  /* extern "C" */

namespace {

int seat2_trace_on(void)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_SEAT2_TRACE") != 0;
    return on;
}

void seat2_note(const char *what, const void *self)
{
    static unsigned char said[2];
    int i = what[0] == 'C' ? 0 : 1;
    if (!seat2_trace_on() || said[i]) return;
    said[i] = 1;
    std::fprintf(stderr, "  [seat2] %s entered for the first time, this=%p\n",
                 what, self);
    std::fflush(stderr);
}

/* ShadowModel's host table is dispatched by MSVC-compiled callers, so every
   word in it is a __fastcall face with a dead edx -- hal/cxxname_bridge.cpp's
   shadow_dtor at word 0 and hal/model_dtor_seat.cpp's shadow_dosetfile at word
   2 are both that shape.  Word 1 matches them.  The receiver is forwarded
   explicitly; nothing here rides a register through. */
void *__fastcall seat2_shadow_d0(void *self, void *)
{
    seat2_note("ShadowModel D0 (_ZTV11ShadowModel+0x04)", self);
    return _ZN11ShadowModelD0Ev(self);
}

/* _ZTV7Clipper's word 0 is the matched _ZN7ClipperD1Ev ITSELF, not a face:
   hal/ctor_runner.cpp writes `(void *)&_ZN7ClipperD1Ev`, a plain C function
   taking the receiver as its first stack argument.  Word 1 is seated the same
   way, through this one-line forwarder only so the trace knob has somewhere to
   live; the forwarder is __cdecl like the body it calls and it passes the
   receiver on. */
void *seat2_clipper_d0(void *self)
{
    seat2_note("Clipper D0 (_ZTV7Clipper+0x04)", self);
    return _ZN7ClipperD0Ev(self);
}

struct Seat2VtableSeat {
    Seat2VtableSeat()
    {
        _ZTV7Clipper[1]      = (void *)&seat2_clipper_d0;
        _ZTV11ShadowModel[1] = (void *)&seat2_shadow_d0;
    }
};
Seat2VtableSeat seat2_vtable_seat;

}  /* anonymous namespace */

/* How a census reads this file's answer: the two words, live, as addresses.
   Nothing in the tree calls it yet; it exists so the seat can be asked about
   rather than assumed, the way port_fader_vtable_seated is. */
extern "C" int port_seat2_words_live(void)
{
    return (_ZTV7Clipper[1] != 0) + (_ZTV11ShadowModel[1] != 0);
}
