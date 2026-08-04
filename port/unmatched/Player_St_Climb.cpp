// HOST BRIDGE for ST_CLIMB (data_ov002_021106dc) -- the state Mario enters
// when he grabs a tree trunk or a pole. Landing on a castle-grounds bush or
// tree canopy runs the actor-286 Tree's CylinderClsn through
// CylinderClsn::Process, whose notify bit reaches func_ov002_020caf98, which
// sets Player+0x37c to the cylinder and changes to this state.
//
// ALL THREE HALVES WERE UNHOSTED, and that is Tango's freeze-then-slide:
//   St_Climb_Init    0x020cbd34  matched src, was in no slice manifest
//   St_Climb_Main    0x020cb5bc  matched src (was the last unhosted half)
//   St_Climb_Cleanup 0x020cb568  matched src, was in no slice manifest
// hal_call_state_fn no-ops what it cannot map, so Init never ran. Init is
// what starts the climb animation (SetAnim 0x26/0x27) and what ZEROES the
// horizontal speed at +0x98/+0x9c and the vertical at +0xa8. Without it the
// anim never changes (the freeze) and he keeps the speed he grabbed at while
// sitting in a state whose Main also does nothing (the slide) -- and the
// fields Init seats (+0x688 the climb height, +0x5c/+0x64 snapped to the
// trunk, +0x6e5 the step) stay whatever they were, which is what a later
// consumer then reads.
//
// St_Climb_Init is matched source but include/Player.h does not declare it,
// so it cannot be called through the port's Player class. The source
// declares its own one-method `class Player`; MSVC's mangling for that is
// the same symbol, so this TU re-declares the identical shadow class -- the
// technique the port already uses for Butterfly::Render's ModelAnim -- and
// hands player_states.inc a plain C entry point. Cleanup needs no bridge:
// include/Player.h line 377 declares it, so the .inc calls it directly.
class Player {
public:
    int St_Climb_Init();
};

extern "C" int port_player_st_climb_init(void *self)
{
    return ((Player *)self)->Player::St_Climb_Init();
}

// ov002 0x020cb5bc, 0x778 bytes: MATCHED as of the div-9 crack (the wall was
// a dropped second argument -- func_ov002_020cc05c takes the pad halfword
// from data_0209f49c, which is what keeps r1 occupied through case 1). The
// source defines an extern "C" function carrying the Itanium name directly,
// so this bridge only forwards. St_Climb_Main promotes to ST_HEADSTAND at
// the top of the cylinder, ST_POLE_JUMP on the jump button, ST_FALL on
// release, and back to ST_WALK when the grab flag at +0x6de clears.
extern "C" int _ZN6Player13St_Climb_MainEv(char *self);

extern "C" int port_player_st_climb_main(void *self)
{
    return _ZN6Player13St_Climb_MainEv((char *)self);
}
