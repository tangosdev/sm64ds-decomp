#include "types.h"
// func_0200e6d8 - find player actor by parameter
struct Actor {
    void* vtable;    // 0x0
    u32 uniqueID;    // 0x4
    u32 param1;      // 0x8
};

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern u8 CURR_PLAYER_ID;
extern struct Actor* PLAYER_ARR[];
extern u32 KS_PLAYER_IDS[];

struct Actor* func_0200e6d8(u32 arg0) {
    struct Actor* player;
    struct Actor* result;

    player = PLAYER_ARR[CURR_PLAYER_ID];
    if (arg0 == player->param1) {
        result = player;
    } else {
        result = _ZN5Actor10FindWithIDEj(KS_PLAYER_IDS[arg0]);
    }
    return result;
}