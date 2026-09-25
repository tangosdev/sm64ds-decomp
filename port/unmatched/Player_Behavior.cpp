// RETIRED (run linkfull, lane PMF3): this file was the host copy of
// src/_ZN6Player8BehaviorEv.cpp with its state-main dispatch replaced by
// hal_call_state_fn on the DS code word at State+8. The matched TU is on
// port/slice_w31_pmf3.txt and dispatches `(this->*(st->mMain))()` as
// written: /vmg /vmm makes the member pointer the ROM's eight-byte pair, and
// port_player_states_seat (hal/pmf3_player_states.cpp) seats every State
// cell with the __fastcall face for its ROM function. It defines nothing.
