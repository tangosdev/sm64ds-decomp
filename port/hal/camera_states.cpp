// The camera state machine, host side.
//
// The ROM keeps 19 State objects in arm9 bss at 0x0209b008..0x0209b128, step
// 0x10, each {PMF onEnter, PMF main} -- two mwcc member-function pointers,
// which are {code address, this-delta} pairs. __sinit_02073a24 fills them by
// copying 38 relocated pairs out of 0x02086bc8..0x02086e48. That sinit is not
// run on host (its 38 source pairs are ROM pointer data that would each need
// its own symbol); the objects are synthesized here with the same addresses,
// read out of the arm9 image once and baked in below.
//
// The addresses stay DS addresses on purpose. Nothing calls through them:
// func_0200cae4 (onEnter) and func_0200ca50 (main), the matched TUs through
// hostgen's MG_PMF_CALL patch (lane shadow-A retired their host copies),
// read the word and hand it to hal_call_camera_state_fn, which translates it
// to a host call through the table in camera_states.inc. That is the same
// shape the player state machine uses (hal_call_state_fn / player_states.inc)
// and it is why an MSVC build can drive a mwcc PMF table at all.
//
// Two more things depend on these objects being ONE contiguous array:
// Camera::InitResources enters &data_0209b008[0], and Camera::ChangeState
// compares the current state against &data_0209b0c8. Both are just symbol
// addresses, so ordinary separate definitions would do -- but every state
// past 0 is reached by an explicit &data_0209bXXX, so each one is named.
#include <cstdio>
#include "dsstate_seg.h"

extern "C" {

/* the 29 distinct state functions, all matched in src/ and all in the
   gate-13 slice. Declared through void* here: they are C-linkage cdecl
   one-argument functions, and their own TUs spell the parameter every way
   from `int` to a local struct pointer. */
int func_0200af0c(void *);
int func_02009e70(void *);
int func_02009e50(void *);
int func_02009e30(void *);
int func_02009e10(void *);
int func_02009df0(void *);
int func_02009dd0(void *);
int func_02009db0(void *);
int func_02009d90(void *);
int func_02009d70(void *);
int func_02009d30(void *);
int func_02009aa8(void *);
int func_020098b8(void *);
void func_0200987c(void *);
int func_0200985c(void *);
int func_020097ec(void *);
int func_020095e4(void *);
int func_02005324(void *);
int func_02005110(void *);
int func_020050dc(void *);
int func_02005098(void *);
int func_020095c4(void *);
int func_02009540(void *);
int func_02009a8c(void *);
int func_020098e0(void *);
int func_020094e4(void *);
void func_020094c0(void *);
int func_02005060(void *);
void func_02005000(void *);

int hal_call_camera_state_fn(void *self, unsigned ds_addr)
{
    switch (ds_addr) {
#include "camera_states.inc"
    }
    std::fprintf(stderr, "  [cam] unhosted camera state fn 0x%08x (no-op)\n",
                 ds_addr);
    return 1;
}

/* The 19 State objects. Layout: {onEnter fn, onEnter delta, main fn, main
   delta}. The deltas are all zero in the ROM's pairs.

   These are routed into the save state's captured section even though nothing
   writes them after boot (the ROM's __sinit_02073a24 fills them once; the port
   bakes the same values in). Capturing 304 bytes that never change is free;
   exempting them would put back the hand-asserted "this one cannot diverge"
   list that the 0.2.0 restore crash came out of. See hal/dsstate_seg.h. */
DSSTATE_BEGIN
#define CAMSTATE(name, enter, main) \
    unsigned name[4] = {enter, 0, main, 0}

CAMSTATE(data_0209b008, 0x0200af0c, 0x02009e70);
CAMSTATE(data_0209b018, 0x02009e50, 0x02009e70);
CAMSTATE(data_0209b028, 0x02009e30, 0x02009e70);
CAMSTATE(data_0209b038, 0x02009e10, 0x02009e70);
CAMSTATE(data_0209b048, 0x02009df0, 0x02009e70);
CAMSTATE(data_0209b058, 0x02009dd0, 0x02009e70);
CAMSTATE(data_0209b068, 0x02009db0, 0x02009e70);
CAMSTATE(data_0209b078, 0x02009d90, 0x02009e70);
CAMSTATE(data_0209b088, 0x02009d70, 0x02009e70);
CAMSTATE(data_0209b098, 0x02009d30, 0x02009aa8);
CAMSTATE(data_0209b0a8, 0x020098b8, 0x0200987c);
CAMSTATE(data_0209b0b8, 0x0200985c, 0x02009e70);
CAMSTATE(data_0209b0c8, 0x020097ec, 0x020095e4);
CAMSTATE(data_0209b0d8, 0x02005324, 0x02005110);
CAMSTATE(data_0209b0e8, 0x020050dc, 0x02005098);
CAMSTATE(data_0209b0f8, 0x020095c4, 0x02009540);
CAMSTATE(data_0209b108, 0x02009a8c, 0x020098e0);
CAMSTATE(data_0209b118, 0x020094e4, 0x020094c0);
CAMSTATE(data_0209b128, 0x02005060, 0x02005000);

#undef CAMSTATE
DSSTATE_END

}  /* extern "C" */
