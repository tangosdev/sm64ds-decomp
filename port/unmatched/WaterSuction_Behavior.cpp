/* HOST COPY of src/_ZN12WaterSuction8BehaviorEv.cpp -- WATERSUCTION's
 * per-frame body (id 247, daWater_Suikomi_c, ov026, vtable slot 6).
 *
 * WHY A HOST COPY, and why this one is worse than the other three. The matched
 * src not only DISPATCHES a real C++ pointer-to-member on a forward-declared
 * class, it EMBEDS one in a struct and reads it at a fixed offset:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct Obj { char pad[8]; PMF pmf; };
 *     struct C { char pad[0x30c]; Obj *obj; };
 *     Obj *o = c->obj;
 *     if (*(int *)((char *)o + 8) != 0) { (c->*(o->pmf))(); }
 *
 * so MSVC's generalised pointer-to-member representation gets the member
 * OFFSET wrong before it gets the call wrong. The ROM's record is the 8-byte
 * Itanium {function, adjustment} pair and `o + 8` is the cell's TICK half.
 * Everything else here is transcribed statement for statement; only that
 * dispatch is spelled as a plain function-pointer call through the record
 * layout hal/actor_classes_ov026.cpp's port_ov026_states_seat() already
 * seated. The matched src stays in src/ as the byte proof and is absent from
 * port/slice_ov026cast.txt.
 *
 * NOT PLACED ON LEVEL 18. This class seats with the rest of ov026's cast and
 * proves by no-regression on the levels that do place it, not by a level-18
 * census move.
 */
#include "WaterSuction.h"

extern "C" {

struct PortOv026Cell { unsigned enter_fn, enter_adj, tick_fn, tick_adj; };
typedef int (*PortOv026Fn)(void *);

unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
void _ZN12CylinderClsn5ClearEv(void *a);
void _ZN12CylinderClsn6UpdateEv(void *a);
void func_ov026_02112324(char *c);

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN12WaterSuction8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

}
