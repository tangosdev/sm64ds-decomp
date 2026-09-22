// port/hal/smoke_host_globals_actor.cpp -- the fifteen host globals and ROM
// helpers of out/SMOKELINK4B/remaining.md section E that are not ordinary SRC
// rows, for smoke_actor, smoke_savestate and smoke_persist.
//
// Lane SMOKELINK5, run link100 wave 10 round 5.
//
// Four of the fifteen (func_02042ffc, func_02043060, func_02043180,
// func_020433b8) are ordinary src/ rows on port/slice_gate10.txt and are added
// to PORT_SMOKE_ACTOR_SRC directly rather than copied here.  _data_020ad524 is
// the ov001 mount, a target_sources line beside port_smoke_relink's call
// rather than a file of its own.  The other ten are host-only storage and two
// ROM-primitive helpers that live in three files these three targets do not
// otherwise carry whole (hal/cxx_aliases.cpp, 16000 lines and real
// definitions as well as rows; hal/camera_bridges.cpp; hal/lk4_solidheap_seat.cpp),
// so each is a verbatim subset in the shape this lane's other subsets use.
//
// EIGHT ROWS FROM hal/cxx_aliases.cpp, copied character for character:
//   data_0209ee74   line 563 (the array), plus the DSSTATE bracket it already
//                   sits inside there (lines ~499 and after)
//   data_0209f274   line 568, same bracket
//   data_020a4b4c, data_020a4b50   line 502, same bracket
//   data_0209b4a4   line 565 (`int data_0209f32c[4], data_0209b4a4[4];` --
//                   only the second half is this batch's row; data_0209f32c
//                   is not on the wall and is not copied), same bracket.
//                   Reached from src/_ZN5Sound4PlayEjjRK7Vector3.cpp, which
//                   this commit also adds (out/SMOKELINK4B/remaining.md
//                   section F).
//   func_01ff98f4, func_01ffa4bc   lines 187-189, ITCM soft-float primitives,
//                   no DSSTATE bracket (functions, not state)
//   func_02052800, func_0205283c  lines 143-155, the X- and Z-rotation matrix
//                   primitives, no DSSTATE bracket
//
// ONE ROW FROM hal/camera_bridges.cpp's DSSTATE bracket (lines ~283-309),
// copied character for character along with the one array it aliases:
//   hal_area_table (static storage) and data_0209f314 = hal_area_table
//   data_0209f43c[0x5c / 4], the Clipper Camera's frustum test runs against
//
// ONE ROW FROM hal/lk4_solidheap_seat.cpp's DSSTATE bracket (line 254):
//   data_020a0ea8, Memory::tmpHeapPtr

#include "dsstate_seg.h"
#include <string.h>

// EVERYTHING BELOW IS FLAT C LINKAGE, the same way hal/cxx_aliases.cpp's own
// copies are: that file opens one extern "C" block near its own top (line 83)
// and does not close it until end of file, which is what keeps its globals
// and functions off MSVC's C++ name decoration. This subset needs its own
// such block for the same reason -- a plain C++ global or function here would
// decorate (measured: round 1 of this batch's build linked ?data_0209ee74@@...
// and ?func_02052800@@... instead of the flat names the wall asks for).
extern "C" {

DSSTATE_BEGIN
void *data_0209ee74[4];
int data_0209f274[8];
int data_020a4b4c, data_020a4b50;
static char hal_area_table[64 * 12];
void *data_0209f314 = hal_area_table;
int data_0209f43c[0x5c / 4];
void *data_020a0ea8;                  /* Memory::tmpHeapPtr */
/* SECOND ORDER, from hal/player_bridges.cpp's own DSSTATE bracket (lines
   2367-2425), copied character for character: func_02043060.c's own row,
   data_020a4b58 with the two neighbours its parent declares on the same
   line. player_bridges.cpp is not on this batch and is far too wide to take
   whole for one row. */
int data_020a4b58[4], data_020a4b68[4], data_020a60f4[4];
/* THIRD ORDER, same file, same bracket: fBase_c::Process's four constants
   (func_0204335c.c calls it) player_bridges.cpp:2416 declares on one line. */
int data_02099e94[4], data_02099ebc[4], data_02099ec4[4], data_02099fcc[4];
DSSTATE_END

/* PORT_HOST_ABI: ARM asm primitive (ITCM soft-float block), MSVC cannot
   assemble -- hal/cxx_aliases.cpp's block comment ahead of its own copy has
   the full derivation (main has matched five of this family's siblings but
   every one is a bannered HAND-ASM PRIMITIVE, still unusable here). */
int func_01ffa4bc(int a) { float f = (float)a; int b; memcpy(&b, &f, 4); return b; }
int func_01ff98f4(int a, int b)
{ float x, y; memcpy(&x, &a, 4); memcpy(&y, &b, 4); return x < y; }

/* PORT_HOST_ABI: ARM/Thumb asm primitive (rotation matrix), MSVC cannot
   assemble. Narrowing the angle parameters to `short` fixes the sign-extend
   trap hal/cxx_aliases.cpp's block comment documents (a `short`-declaring
   caller pushes exactly the bits an `int`-declaring one reads); every caller
   in the tree passes an entry of the unit sine table, [-4096, 4096], so the
   narrowing is lossless. */
void func_02052800(int *m, short s, short c)   /* X rotation */
{
    m[0] = 0x1000; m[1] = 0; m[2] = 0;
    m[3] = 0; m[4] = c; m[5] = s;
    m[6] = 0; m[7] = -s; m[8] = c;
    m[9] = 0; m[10] = 0; m[11] = 0;
}
/* PORT_HOST_ABI: ARM/Thumb asm primitive (Z-rotation matrix), MSVC cannot assemble. */
void func_0205283c(int *m, short s, short c)   /* Z rotation */
{
    m[0] = c; m[1] = s; m[2] = 0;
    m[3] = -s; m[4] = c; m[5] = 0;
    m[6] = 0; m[7] = 0; m[8] = 0x1000;
    m[9] = 0; m[10] = 0; m[11] = 0;
}

}  // extern "C"
