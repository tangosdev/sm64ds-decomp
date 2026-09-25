// RETIRED (run linkfull, lane PMF3): this file was the host copy of
// src/_ZN6Player11ChangeStateERNS_5StateE.cpp with its two member-pointer
// dispatch sites (the old state's Cleanup at +0x10, the new state's Init at
// +0) replaced by hal_call_state_fn. The matched TU is on
// port/slice_w31_pmf3.txt and calls both as written, through the State cells
// port_player_states_seat (hal/pmf3_player_states.cpp) seats; the flat ROM
// name _ZN6Player11ChangeStateERNS_5StateE the C callers ask for is its
// reverse face in port/faces_sync.txt. It defines nothing.
