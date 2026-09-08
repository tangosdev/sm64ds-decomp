/* HOST COPY of src/_ZN9Submarine8BehaviorEv.cpp -- the WHIRLPOOL's per-frame
 * body (id 246, daWater_Tatumaki_c, ov026, vtable slot 6).
 *
 * THE NAME IS dsd's, NOT THE CLASS'S. Every `_ZN9Submarine*` TU in ov026 is
 * the whirlpool's: table 0x02113d54 carries this body at slot 6, and that
 * table's own vtable[-1] __si_class_type_info record's name string reads
 * "18daWater_Tatumaki_c". The real Submarine (id 90) is table 0x02113c6c and
 * has NO Behavior of its own -- its slot 6 is ActorBase::Behavior. See
 * hal/actor_classes_ov026.cpp's header for the full derivation.
 *
 * WHY A HOST COPY. The matched src's state tick is a REAL C++ pointer-to-member
 * call on a forward-declared class:
 *
 *     class C; typedef int (C::*PMF)(); class C { public: int d; };
 *     char *obj = *(char **)&unk_110;
 *     if (*(int *)(obj + 8) != 0) { PMF *pp = (PMF *)(obj + 8);
 *                                   ((C *)this->**pp)(); }
 *
 * -- the SoundObject/Cap/MrBlizzard/BabyPenguin disease. MSVC's PMF for an
 * incomplete class is the generalised multi-word form and does not reproduce
 * the ROM's 8-byte {function, adjustment} record. Everything else in this body
 * is transcribed statement for statement from the matched source; only that one
 * dispatch is spelled as a plain function-pointer call through the same record
 * layout port_ov026_states_seat() already seated. The matched src stays in src/
 * as the byte proof and is absent from port/slice_ov026cast.txt.
 *
 * THE CELL POINTER at +0x110 is written by func_ov026_02111ee0 (host-copied in
 * Whirlpool_StateDispatch.cpp); the tick half is the record at cell+8. The
 * whirlpool's InitResources enters state 0 (data_ov026_02113f2c) and its
 * state-0 tick func_ov026_02111d4c enters state 1 (data_ov026_02113f3c) when a
 * player comes inside 0x12c000 horizontally and vertically.
 */
#include "Submarine.h"

extern "C" {

struct PortOv026Cell { unsigned enter_fn, enter_adj, tick_fn, tick_adj; };
typedef int (*PortOv026Fn)(void *);

unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *cc);
void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int fx, int t1, int t2, int s4, int s5);
void _ZN9Animation7AdvanceEv(void *thiz);
void func_ov026_02111f30(char *c);

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN9Submarine8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

}
