/* KLEPTO (actor 239, ov062 _ZTV6Klepto / _ZTV9daJango_c 0x0211dd5c): the
 * SEAT of its ten state-handler template records, the faces the matched
 * per-frame dispatch in src/_ZN6Klepto8BehaviorEv.cpp calls into
 * (port/slice_w24_faceflip.txt).
 *
 * KLEPTO drives a five-state machine through five 16-byte descriptors in ov062
 * bss (data_ov062_0211e14c/15c/16c/17c/18c), each two mwcc {function, delta}
 * records: the ENTER half at +0 and the MAIN half at +8. func_ov062_0211c658
 * (matched, port/slice_pmf2.txt) stores a descriptor pointer at this+0x42c and
 * dispatches the ENTER half once; Klepto::Behavior dispatches the MAIN half
 * every frame.
 *
 * WHAT THE ROM DOES WITH A RECORD (run linkfull lane PMF1, disassembled out of
 * extracted/overlays/overlay_0062.bin at 0x0211c968..0x0211c998): the ARM C++
 * ABI member-pointer call, word 0 tested for null, receiver = this +
 * (word 1 >> 1), bit 0 of word 1 selecting a virtual call through the
 * receiver's vtable, else word 0 called directly. The instruction sequence is
 * written out in port/unmatched/HootTheOwl_StateDispatch.cpp's header; Hoot's
 * Behavior carries the same one. MSVC's member pointer under /vmg /vmm
 * (target-wide, port/CMakeLists.txt block R8) is the same eight bytes in the
 * same order and calls `ecx = this + word 1; call word 0`, which agrees with
 * the ROM exactly when word 1 is zero. It is zero in all ten ROM records:
 * each +4 word of 0x0211dcd0..0x0211dd1c reads 00000000 in the image and no
 * relocation is recorded from any of them (config/arm9/overlays/ov062/
 * relocs.txt; runs/linkfull/out/PMF1/rom_klepto_records.txt). The seat below
 * aborts on any nonzero word 1 as well.
 *
 * THE RECEIVER RIDES IN ECX. MSVC's call through a member pointer puts
 * this + word 1 in ecx and pushes NOTHING, both in Behavior's call and in
 * func_ov062_0211c658's tail jump. The ten handlers are extern "C" cdecl and
 * read their receiver from [esp+4], so every record, enter and main alike, is
 * seated with a __fastcall face that takes ecx and calls the handler with it
 * (the Pokey / Ukiki shape; port/tools/pmf_guard.py's rule puts the face on
 * every row).
 *
 * THE SEAT (the Chuckya "seat the source before the copy" order)
 * --------------------------------------------------------------
 * The ten SOURCE records live at ov062 0x0211dcd0..0x0211dd18 (8 bytes each);
 * __sinit_ov062_0211d6fc copies them into the five bss descriptors. The ovdata
 * mount lays the source blocks down with their fn words as DS CODE ADDRESSES
 * (the mount rebases only pointers into other mounted DATA).
 * port_klepto_states_seat() rewrites each source fn word with its face BEFORE
 * that sinit runs, validating each mounted word against the ROM address first
 * (WRONG-BYTES abort). All ten handlers are matched src (slice_klepto.txt).
 * Called from hal/actor_overlays.cpp between port_chuckya_states_seat() and
 * __sinit_ov062_0211d6fc().
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* the ten state-handler template records (mounted, {fn, delta}) */
extern PortPmf data_ov062_0211dcd0, data_ov062_0211dcd8, data_ov062_0211dce0,
    data_ov062_0211dce8, data_ov062_0211dcf0, data_ov062_0211dcf8,
    data_ov062_0211dd00, data_ov062_0211dd08, data_ov062_0211dd10,
    data_ov062_0211dd18;

/* the ten matched handlers (cdecl, receiver as the first stack argument) */
int func_ov062_0211ba84(void *c); int func_ov062_0211c594(void *c);
int func_ov062_0211bd10(void *c); int func_ov062_0211b880(void *c);
int func_ov062_0211c2f4(void *c); int func_ov062_0211bc54(void *c);
int func_ov062_0211b8d8(void *c); int func_ov062_0211b800(void *c);
int func_ov062_0211c218(void *c); int func_ov062_0211b930(void *c);

}  /* extern "C" */

/* func_ov062_0211c658 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov062_0211c658.cpp back on port/slice_pmf2.txt (batch 2): with /vmg
   /vmm global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair and the matched TU compiles to a tail jump that sets ecx first. */

/* Klepto::Behavior IS NOT HOST-COPIED ANY MORE. src/_ZN6Klepto8BehaviorEv.cpp is
   on port/slice_w24_faceflip.txt: its dispatch compiles to
   `mov ecx,[rec+0Ch] / add ecx,this / call [rec+8]` under this build's flags
   (runs/linkfull/out/PMF1/msvc_klepto_behavior.asm), which the faces below
   take in ecx, and port/faces_sync.txt's reverse face defines the flat
   _ZN6Klepto8BehaviorEv that hal/actor_classes_koopa_chuckya.cpp's slot
   thunk calls. */

/* ---- THE SEAT -------------------------------------------------------------
   The ten source fn words (0x0211dcd0..0x0211dd18), each {slot, rom, face}
   matched against its own reloc destination in
   config/arm9/overlays/ov062/relocs.txt. Rewritten BEFORE
   __sinit_ov062_0211d6fc copies them into the five bss descriptors. */
#define KLEPTO_FACE(a) static int __fastcall kl_st_##a(void *s) \
    { return func_ov062_##a(s); }
KLEPTO_FACE(0211ba84) KLEPTO_FACE(0211c594) KLEPTO_FACE(0211bd10)
KLEPTO_FACE(0211b880) KLEPTO_FACE(0211c2f4) KLEPTO_FACE(0211bc54)
KLEPTO_FACE(0211b8d8) KLEPTO_FACE(0211b800) KLEPTO_FACE(0211c218)
KLEPTO_FACE(0211b930)
#undef KLEPTO_FACE

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_klepto_states[] = {
    {&data_ov062_0211dcd0, 0x0211ba84, (void *)kl_st_0211ba84},
    {&data_ov062_0211dcd8, 0x0211c594, (void *)kl_st_0211c594},
    {&data_ov062_0211dce0, 0x0211bd10, (void *)kl_st_0211bd10},
    {&data_ov062_0211dce8, 0x0211b880, (void *)kl_st_0211b880},
    {&data_ov062_0211dcf0, 0x0211c2f4, (void *)kl_st_0211c2f4},
    {&data_ov062_0211dcf8, 0x0211bc54, (void *)kl_st_0211bc54},
    {&data_ov062_0211dd00, 0x0211b8d8, (void *)kl_st_0211b8d8},
    {&data_ov062_0211dd08, 0x0211b800, (void *)kl_st_0211b800},
    {&data_ov062_0211dd10, 0x0211c218, (void *)kl_st_0211c218},
    {&data_ov062_0211dd18, 0x0211b930, (void *)kl_st_0211b930},
};

extern "C" void port_klepto_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_klepto_states /
                             sizeof g_klepto_states[0]; ++i) {
        PortPmf *p = g_klepto_states[i].slot;
        if (p->fn != g_klepto_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: KLEPTO state template %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, p->fn, p->delta,
                         g_klepto_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_klepto_states[i].host;
    }
}
