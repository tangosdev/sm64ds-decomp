/* HOST COPY RETIRED, run link100 wave 7 lane SEAT5, census batch B5.
 * src/_ZN11MirrorLuigi8BehaviorEv.cpp is on port/slice_seat5.txt and runs the
 * ROM's own tick dispatch itself. What is left in this file is the two lines
 * the port still owes that TU: the flat C name the actor class table calls, and
 * the ONE __fastcall face the cell's TICK word has to hold once the matched TU
 * is the reader.
 *
 * WHY THE BANNER THIS FILE CARRIED IS DEAD. It said mwcc's pointer to member
 * over the deliberately incomplete `struct C5` is the ROM's 8-byte
 * {function, delta} pair while MSVC's is the "unknown inheritance" form. Block
 * R8 turns on /vmg AND /vmm for every C++ source in this build and /vmm picks
 * the multiple-inheritance worst case, which IS eight bytes. The TU's own /FAsc
 * listing under the port's flags (runs/link100/out/SEAT5/emit_seat5_out.txt) is
 *
 *     mov  eax,DWORD PTR [ecx+8]     the TICK half's code word, ROM offset +8
 *     test eax,eax / je              the ROM's own null guard
 *     mov  ecx,DWORD PTR [ecx+12]    the adjust word at cell+0xc
 *     push ebx                       ONE stack argument, the player pointer
 *     add  ecx,edi                   receiver = this + adjust
 *     call eax                       a CALL; the caller does NOT clean up
 *
 * -- the ROM's own offsets, receiver in ecx, ARITY ONE, and the one pushed word
 * is never popped by the caller because __thiscall's stack half is callee
 * popped. /Zp4 changes 0 listing lines outside the TITLE. So the face is
 * __fastcall(self, dead_edx, void *val), lane FWD's MgCup shape and PMFB8's
 * gate-1 shape one word wider than PMFB7's twelve.
 *
 * THE UNIVERSE IS CLOSED AND WAS ALREADY MEASURED. Lane PMFB8 swept
 * MirrorLuigi's class code block (021111a0..02111860, every ov055 function
 * symbol whose mangled name names the class) and found exactly TWO {code,0}
 * pairs, 02111a94 = 021112bc (the ENTER half) and 02111a9c = 02111288 (the TICK
 * half), a solid run 02111a94..02111aa4 whose only references are
 * __sinit_ov055_021118d4's own two loads, which copy both into the single
 * sixteen-byte cell data_ov055_02111b70. ONE cell, TWO halves, TWO readers, and
 * they are DISJOINT RECORDS: the ENTER word is read by
 * src/func_ov055_021112c4.cpp (seated by PMFB8, already a __fastcall face in
 * hal/actor_classes_ov055.cpp) and the TICK word is read by this row and by
 * nothing else. Nothing compares a stored pair by value, so there is no
 * sentinel and no one-face-per-code-word constraint.
 *
 * WHY THE FACE IS INSTALLED FROM THE FORWARDER RATHER THAN FROM THE SEAT.
 * port_mirrorluigi_state_seat(), which writes both words of the cell, lives in
 * port/hal/actor_classes_ov055.cpp, which is NOT this lane's file. The
 * equivalent one-line change there (point cell.tick_fn at this face instead of
 * at func_ov055_02111288) is the cleaner shape and is written up for the fold
 * in runs/link100/out/SEAT5/decomp_side.md. Installing it here instead is
 * EXACT rather than merely close, because this forwarder is the only path to
 * the TICK word in the whole binary: vtable slot 6 is the sole caller of
 * _ZN11MirrorLuigi8BehaviorEv (hal/actor_classes_ov055.cpp's ml_behavior), the
 * matched TU is the sole reader of the TICK word, and the swap therefore
 * happens strictly before the first dispatch through it. The ROM value is
 * checked before the swap, so a cell that did not come out of
 * port_mirrorluigi_state_seat aborts instead of being overwritten.
 */
#include <cstdio>
#include <cstdlib>
#include "MirrorLuigi.h"

extern "C" {
struct PortMirrorLuigiCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortMirrorLuigiCell data_ov055_02111b70;
int func_ov055_02111288(void *dst, void *src);
}

/* the TICK half: func_ov055_02111288, the mirror of the player's position, as
   the zero-stack-cleanup __fastcall face the matched TU's `call eax` needs. */
static int __fastcall ml_tick_face_02111288(void *self, void *dead_edx,
                                            void *val)
{
    (void)dead_edx;
    return func_ov055_02111288(self, val);
}

extern "C" int _ZN11MirrorLuigi8BehaviorEv(void *selfv)
{
    static int seated;
    if (!seated) {
        seated = 1;
        PortMirrorLuigiCell &cell = data_ov055_02111b70;
        if (cell.tick_fn != (unsigned)(size_t)&func_ov055_02111288) {
            std::fprintf(stderr, "FATAL: MirrorLuigi TICK word: the cell holds "
                         "%08x, port_mirrorluigi_state_seat's own host body is "
                         "%08x -- WRONG BYTES, refusing to seat the face\n",
                         cell.tick_fn,
                         (unsigned)(size_t)&func_ov055_02111288);
            std::abort();
        }
        cell.tick_fn = (unsigned)(size_t)&ml_tick_face_02111288;
    }
    return ((MirrorLuigi *)selfv)->MirrorLuigi::Behavior();
}
