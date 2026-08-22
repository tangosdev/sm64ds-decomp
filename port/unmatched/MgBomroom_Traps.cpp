// PORT_HOST_ABI. dScMgBomroom_c's one named trap. Run mg6, lane SOS.
//
// ---- THE ONE TRAP, AND IT IS A DECOMP FLOOR --------------------------------
//
// func_ov006_020d8408 is the second of the four calls state 2 of
// data_ov006_021416e0 makes. src/func_ov006_020d8ff4.c is that state and it
// names the symbol; config/arm9/overlays/ov006/symbols.txt names it and sizes
// it 0x4fc; and there is NO delink block covering it in
// config/arm9/overlays/ov006/delinks.txt and NO src file in either extension
// -- all three checked rather than assumed. So dScMgBomroom_c cannot do
// everything state 2 does, however this seat is written.
//
// IT IS THE BIGGEST BODY THE CLASS OWNS THAT NOBODY HAS DECOMPILED. 0x4fc is
// larger than the class's own InitResources (0x330), so a lane reading the
// census should not mistake this for a leaf: it is the second call of the
// state the class sits in after its intro, and the trap firing means real game
// logic did not run. The count is reported by name in
// hal/scene_mg_bomroom.cpp's census whether or not it fires, so a zero says
// which zero it is.
//
// It gets a trap rather than a transcription for the reason
// hal/scene_mg_faces.cpp's ov004 traps and unmatched/MgCoin_Faces.cpp's did: a
// body that returns 0 and says so is honest, and a plausible one is the guess
// port/tools/inferred_stub_guard exists to refuse. Transcribing it from the
// ROM is a follow-up of the shape lane CT1 did for curling's 0x020e1854, and
// it is deliberately not this lane's, because that lane's own banner is the
// rule: the host body would have to be named port_* rather than
// func_ov006_020d8408, and nothing would then satisfy the src caller.
//
// THIS CLASS'S SECOND FLOOR IS NOT HERE. func_ov006_020d7c4c is a STATE with
// no body, and it is handled at the dispatch site in
// unmatched/MgBomroom_StateDispatch.cpp, where the report can say which state
// was wanted. It has no symbol here on purpose: no host code names that
// address except the switch, so a trap would be a definition nobody calls.

#include <cstdio>

static unsigned g_bomroom_trap_hits;

extern "C" int func_ov006_020d8408(void *)
{
    static int said;
    ++g_bomroom_trap_hits;
    if (!said) {
        said = 1;
        std::fprintf(stderr,
                     "  [scene] TRAP func_ov006_020d8408: no delink block and "
                     "no src TU (ov006, 0x4fc). dScMgBomroom_c state 2 calls "
                     "it and it does nothing here.\n");
        std::fflush(stderr);
    }
    return 0;
}

extern "C" unsigned port_mg_bomroom_trap_hits(void) { return g_bomroom_trap_hits; }
