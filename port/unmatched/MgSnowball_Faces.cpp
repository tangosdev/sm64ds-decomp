// PORT_HOST_ABI. dScMgSnowball_c's vtable face and its three closure floors.
// Run mg11, lane SNW. Actor id 0x179 = scene 377, "Snowball Slalom".
//
// ---- 1. ONE ALIAS, AND IT IS THE ORDINARY KIND ---------------------------
//
// src/func_ov006_021295ac.cpp -- this class's constructor, the body that
// resolves the vtable section 3 of port/mg_fanout_costs.txt left blank -- is a
// //cpp TU and declares both tables as
//
//     extern void* data_ov006_0213e448;
//     extern void* data_ov006_0214000c;
//
// so MSVC mangles the references as ?data_ov006_0213e448@@3PAXA and
// ?data_ov006_0214000c@@3PAXA -- C++-linkage void pointers -- while the ov006
// mount (port/tools/ovdata.py's emission) defines the plain C names. That is
// the "C-named symbols declared at C++ linkage" case port/mg_fanout_costs.txt
// section 4 counts twenty-two of, and an alias is the whole fix: both sides
// are the same object at the same address and nothing about the type or the
// calling convention differs.
//
// ONLY ONE OF THE TWO IS HERE, and the reason is worth a line because it looks
// like an omission. The first link of this seat asked for
// ?data_ov006_0214000c@@3PAXA and NOT for the middle base's, because an
// earlier seat under dScMgSingle3DBase_c already stands an alias for
// ?data_ov006_0213e448@@3PAXA. An /alternatename with the same left and right
// sides declared twice is deduplicated by the linker, so a duplicate would be
// harmless -- but a row nothing asks for is a row nobody can tell is doing
// anything, so this file carries the one the link named.
//
// IT IS CHECKED THE OTHER WAY ROUND RATHER THAN TAKEN ON TRUST.
// port/tools/alternatename_guard.py runs POST-LINK over the real map on every
// build and fails if an /alternatename left-hand side is also a defined
// symbol, which is the failure mode that would make this line silently
// useless.
//
// facegen was NOT run to generate this, for the reason
// port/mg_fanout_costs.txt section 10 records: a failed link truncates
// walk_window.map to zero bytes, and that map is the universe facegen
// classifies against, so it refuses after a probe link. The refusal is correct
// and the row is hand-written instead.

#pragma comment(linker, "/alternatename:?data_ov006_0214000c@@3PAXA=_data_ov006_0214000c")

// ---- 2. THREE CLOSURE FLOORS, TRAPPED HONESTLY ---------------------------
//
// The vtable's nosrc column is ZERO: all sixteen override slots resolve to a
// matched src TU.  These three are not vtable slots.  They are bodies this
// class's own closure CALLS, and none has a src TU under any naming
// convention -- the whole of src/ contains no file for any of the three
// addresses.
//
//   func_ov006_02125f68   0x9e0, 617 instructions plus a 15-word pool = 632
//   func_ov006_02126ee4   0xacc, 688 instructions plus a  3-word pool = 691
//   func_ov006_02126b4c   0x398, 227 instructions plus a  3-word pool = 230
//
// ALL THREE ARE THIS CLASS'S ALONE, which is why they are trapped in this
// lane's own file rather than in the shared hal/scene_mg_faces.cpp.  Their
// complete caller sets in ov006 are:
//
//   0x02125f68  from:0x021286d8 and from:0x02128b24, both inside
//               func_ov006_021283a4 (vtable slot 6), in two different arms of
//               its six-arm phase switch.
//   0x02126ee4  from:0x02129250 inside func_ov006_0212921c (vtable slot 18,
//               only past its argument == 0x13 test) and from:0x0212953c
//               inside func_ov006_02129268 (vtable slot 0, InitResources).
//   0x02126b4c  from:0x02126ad4 and from:0x02126b30, both inside
//               func_ov006_02126a98, which slots 0, 9 and 18 all call.
//
// Nothing else in the overlay reaches any of them, so no other lane's merge
// has an opinion about where they live and the mg9 BOO/S371 LNK2005 shape
// cannot repeat here.
//
// THE THIRD ONE ARRIVED IN WAVE 2 AND THAT IS THE TRAP RULE FIRING.
// port/mg_fanout_costs.txt records that a trap-shaped floor HIDES ITS CALLEES
// FROM STATIC CLOSURE.  func_ov006_02126b4c is not called by any vtable slot:
// it is called by func_ov006_02126a98, which wave 1 pulled into the slice, and
// it only became visible to the link once that body was compiling.  A lane
// that budgets one wave should expect a second when it traps anything.
//
// WHAT THEY ARE, from the ROM, so the trap names are not invented:
//
//   func_ov006_02126ee4 CALLS EXACTLY ONE THING, ELEVEN TIMES: 0x0203b990,
//   RandomIntInternal.  Nothing else.  Called once from InitResources and once
//   from the state reset, it is the course layout -- eleven random draws and
//   the writes that place them.
//
//   func_ov006_02125f68 opens by clearing three per-element byte arrays at
//   this+0xabf8, this+0xac18 and this+0xac38 in a counted loop, then runs the
//   fixed-point and vector helpers 0x0203d614 (x4), 0x0203d388 (x4),
//   0x0203d434 (x3) and 0x0203d630 (x2), the particle entry points 0x02022e68
//   and 0x02022d80 (x2 each), the sound call 0x02012718 (x3) and two of this
//   class's own bodies (0x02125bbc, 0x02125cdc, 0x021259d8).  It is the
//   obstacle/collision pass slot 6 runs in two of its phases.
//
//   func_ov006_02126b4c takes (object, int, int) -- its caller's src spells it
//   that way and the ROM tests r2 against 1 on its second instruction -- and
//   calls exactly three things: MultiStore16 (0x0205a448) sixteen times, and
//   G2::GetBG2ScrPtr (0x020550bc) and G2S::GetBG2ScrPtr (0x0205503c) eight
//   times each.  It writes tilemap entries into BOTH screens' BG2 screen
//   blocks, which is this class's 2D digit and tile writer.
//
// A TRAP COUNTS AND RETURNS.  It does not guess a body, and
// port/mg_fanout_costs.txt's rule that a trap-shaped floor HIDES ITS CALLEES
// FROM STATIC CLOSURE applies to all three: the three ov006 bodies 0x02125f68
// calls (0x021259d8, 0x02125bbc, 0x02125cdc) are not in this seat's slice
// because nothing else asks for them, and a future decomp of it will need
// them.
//
// WHAT THE PLAYER SEES.  These are the three honest gaps in this seat.  The
// course is laid out by a floor, the obstacle pass is a floor, and the 2D tile
// writer is a floor, so scene 377 is expected to boot, run its state machine
// and draw its 3D, with the rocks message 554 promises and part of the 2D
// layer missing or inert.  That is stated here rather than discovered by eye.

#include <cstdio>

static unsigned g_snw_floor_02125f68;
static unsigned g_snw_floor_02126ee4;
static unsigned g_snw_floor_02126b4c;

extern "C" void func_ov006_02125f68(char *c)
{
    (void)c;
    ++g_snw_floor_02125f68;
}

extern "C" void func_ov006_02126ee4(void *c)
{
    (void)c;
    ++g_snw_floor_02126ee4;
}

extern "C" void func_ov006_02126b4c(char *c, int a, int b)
{
    (void)c; (void)a; (void)b;
    ++g_snw_floor_02126b4c;
}

extern "C" void port_mg_snowball_floor_counts(unsigned *obstacles,
                                              unsigned *layout,
                                              unsigned *tiles)
{
    if (obstacles) *obstacles = g_snw_floor_02125f68;
    if (layout)    *layout    = g_snw_floor_02126ee4;
    if (tiles)     *tiles     = g_snw_floor_02126b4c;
}

/* This symbol exists so a reader grepping for who owns the alias above finds a
   definition rather than only a comment. */
extern "C" const char port_mg_snowball_faces_marker[] =
    "dScMgSnowball_c faces and floors, run mg11 lane SNW";
