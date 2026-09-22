// port/hal/smoke_player_oam_bridge.cpp -- ONE row, smoke_player ONLY.
//
// Lane SMOKELINK5B, run link100 wave 10 round 5, continuing SMOKELINK5's
// step 2 (smoke_player, 363 -> 42 -> 5 per port/CMakeLists.txt's own
// SMOKELINK5 comment block above the target_sources call this file joins).
// This closes the third of the five residual names; the other two
// (Stage::Behavior, Stage::Render) are the lane FRAME/RENDER9 scope boundary
// documented in this file's neighbourhood in port/CMakeLists.txt and stay
// open, not guessed at.
//
// THE NAME. DrawOamSprite.cpp and _ZN5HUD15RenderCoinCountEv.cpp both declare
// the seven-argument OAM::Render as a free function (no receiver), which MSVC
// mangles ?Render@OAM@@YAX_NPAUOamAttr@@HHHHPAUMatrix2x2@@@Z (YA, void,
// __cdecl). port/hal/sub_actors.cpp:1032 already carries the bridge for it,
// /alternatename:?Render@OAM@@YAX..=?Render@OAM@@SAH.. (the STATIC-MEMBER INT
// spelling), and sub_actors.cpp is on all three targets that build the HUD
// (both walk_window targets and smoke_player). On the two walk_window targets
// the SAH body is port/hal/stage_frame.cpp's `int OAM::Render(...)`, which is
// NOT on smoke_player (documented a few hundred lines up: "GATE 220, lane
// FRAME... on the TWO walk_window targets and NOT on smoke_player", because
// Stage::Behavior's own closure -- the message pump, the pause screen -- has
// never been hosted by this harness). So on smoke_player the ALREADY-PRESENT
// bridge's right-hand side is itself undefined, and the LHS comes back
// unresolved by its own YA spelling, exactly as the link log shows.
//
// SMOKELINK5 already tried the obvious next step -- a SECOND, competing
// /alternatename for the same LHS pointing at the VOID static spelling
// instead -- and measured that it does not fire (port/CMakeLists.txt's
// "NOT PLACED" note, tried and reverted). That is expected: link.exe accepts
// one /alternatename per undefined symbol, and the FIRST one the linker reads
// (sub_actors.cpp's, unconditionally present) is the one that stands, so a
// second directive for the same LHS is silently inert. Editing or removing
// sub_actors.cpp's row is not an option either: it is shared by all three
// targets and the two walk_window ones need exactly that bridge.
//
// THE FIX is therefore not a new alias but a body: this file gives
// smoke_player its OWN definition of the SAH spelling the existing,
// unconditional bridge already asks for, so that bridge resolves here too
// instead of needing a second one. The body is a VERBATIM copy of
// hal/stage_frame.cpp's `int OAM::Render(...)` (proved correct on both
// walk_window targets already), forwarding to the matched flat body
// src/_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2.cpp, which port/slice_gate27.txt
// puts on smoke_player too (the SLICE27_SOURCES list a few hundred lines above
// smoke_player's add_executable's close). Isolated into its own one-function
// TU rather than adding stage_frame.cpp itself, which would drag in
// SLICE220/SLICE_SLOT9 and the rest of Stage's closure this harness does not
// host.
//
// __cdecl on both sides, no receiver, same seven arguments as the recovered
// declaration: a call and nothing else, matching every OAM::Render bridge
// already in this tree.
struct OamAttr;
struct Matrix2x2;

struct OAM {
    static int Render(bool, OamAttr *, int, int, int, int, Matrix2x2 *);
};

extern "C" void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int draw, void *obj,
                                                         int px, int py,
                                                         int pal, int prio,
                                                         void *mtx);

int OAM::Render(bool draw, OamAttr *obj, int px, int py, int pal, int prio,
                Matrix2x2 *mtx)
{
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(draw ? 1 : 0, (void *)obj, px, py,
                                             pal, prio, (void *)mtx);
    return 0;
}
