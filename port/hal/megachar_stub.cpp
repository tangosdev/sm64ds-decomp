// UpdateKillByMegaChar: STUBBED for gate 9, and gate 9 is now all this file is
// for. RETIRED FROM THE REAL TARGETS by lane w8-shadows -- see the gate-16 flip
// at the bottom of hal/wf_enemy_bridges.cpp, which owns the ROM's C name in
// smoke_player, walk_window and walk_window_hires and forwards it into the
// matched TU. This file is compiled only into the three narrow gate-9
// harnesses (smoke_actor, smoke_savestate, smoke_persist), which link no
// Player gate, no RaycastLine and no ov002 body, and where the real closure
// therefore cannot be built. With no mega char in play the real function takes
// its early-out, which is what 0 means, so the harnesses see the behaviour
// they always did.
//
// RULED (w6-c item 3), AND THE RULING HELD: THIS WAS NEVER A HOST-ABI
// EXCEPTION AND MUST NOT BE TAGGED AS ONE. Nothing about the ROM's argument
// passing, its pointers to members or its hardware is unrepresentable here. It
// was a stub standing in for a matched TU that could link, which is the
// definition of a SHADOW, and a PORT_HOST_ABI tag would have moved it out of
// the replacement work list -- the one thing the queue exists to prevent. It
// stayed in the queue until the seat landed, which is the outcome that ruling
// was protecting.
//
// THE COST ESTIMATE THAT USED TO SIT HERE WAS WRONG. It said three of the
// matched TU's nine externals were missing from the binary and named two of
// them _ZN11RaycastLine14SetObjAndLineERK7Vector3S2_P5Actor and
// _ZN11RaycastLine11DetectClsnEv. Those Itanium length prefixes contradict the
// identifiers they precede (SetObjAndLine is 13 characters, DetectClsn is 10),
// so neither string is any symbol, and the real 13/10 spellings were linked all
// along -- from _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c.cpp.obj
// and reverse_bridges.cpp.obj. Only func_ov002_020ee5d0 was genuinely absent,
// and it was already sliced (slice_gate16.txt line 302) and only wanted a
// reference edge. The seat cost one face, not a lane.
extern "C" int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(
    void *, short, short, short, int)
{
    return 0;
}
