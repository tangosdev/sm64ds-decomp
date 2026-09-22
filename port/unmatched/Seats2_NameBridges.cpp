/* port/unmatched/Seats2_NameBridges.cpp -- lane SEATS2, run link100 wave 9b,
 * GATE 3 ONLY.
 *
 * RECONSTRUCTED AT THE FOLD (lane INT3, 2026-09-14) for the same reason
 * port/slice_seats2.txt is: the coordinator's FOLD RULING takes gate 3 and
 * drops gates 1 and 2, and gate 3's commit only ADDS to a file gate 1 wrote.
 * The sixteen decorated ROM data bridges gate 1 put here belong to the three
 * actor TUs lane HOSTGEN4 seats instead, and HOSTGEN4 carries its own rows for
 * them, so none of them is reproduced. What follows is gate 3's block, taken
 * verbatim from b214e6def.
 */

/* GATE 3, THE ACCESS-LETTER BRIDGES.
 *
 * Three rows where the link asks for a member under one MSVC mangle and
 * defines it under another whose ONLY difference is the access and
 * virtualness letter:
 *
 *     ?Behavior@BowserPuzzleManager@@QAEHXZ   asked for
 *     ?Behavior@BowserPuzzleManager@@UAEHXZ   defined
 *
 * Both are AE, __thiscall, with the same signature and the same receiver in
 * ECX, so this is a pure NAME bridge and the objection that stops an
 * /alternatename being used for a member -- the receiver crossing from the
 * stack to ECX -- does not arise. The tree already ships this exact row
 * shape: port/hal/cxx_aliases.cpp bridges ?Virtual34@fBase_c@@UAEHII@Z to
 * ?Virtual34@fBase_c@@QAEHII@Z, and Virtual38 beside it.
 *
 * THE CAUSE, because the bridge is a repair and not a fix. A port bridge
 * file or a per-function src TU declares its own local shadow of the class
 * with the member NON-virtual, from a time when the decomp header did not
 * declare that member at all:
 *
 *     port/hal/fwd_forwarders.cpp:68
 *     struct BowserPuzzleManager { int Behavior(); };
 *
 * and the comment three lines above it says why: "include/
 * BowserPuzzleManager.h and include/Dorrie.h do NOT declare". They do now --
 * include/BowserPuzzleManager.h:25 is `virtual s32 Behavior();` -- so the
 * shadow and the header disagree and the two spellings never meet. Deleting
 * the shadow and including the real header is the fix, and it retires the
 * bridge; that file is not this lane's, so the bridge is what ships today
 * and out/SEATS2/HANDOFF.md carries the fix for whoever owns it.
 *
 * Each left-hand side was checked NOT defined and each right-hand side
 * checked DEFINED in walk_window's own 8670 link inputs.
 */
#pragma comment(linker, "/alternatename:?Behavior@BowserPuzzleManager@@QAEHXZ=?Behavior@BowserPuzzleManager@@UAEHXZ")
#pragma comment(linker, "/alternatename:?Behavior@TTC_MovingBar@@UAEHXZ=?Behavior@TTC_MovingBar@@QAEHXZ")
/* This one only fires because gate 3 also seats
 * src/_ZN14TTC_MovingBeam8BehaviorEv.cpp, which is what defines the
 * right-hand side. Seat and bridge are one change. */
#pragma comment(linker, "/alternatename:?Behavior@TTC_MovingBeam@@UAEHXZ=?Behavior@TTC_MovingBeam@@QAEHXZ")

/* A translation unit with no code in it still has to have something to
 * compile, and the pragmas above are directives rather than definitions. */
extern "C" int port_seats2_name_bridges_present;
int port_seats2_name_bridges_present = 1;
