#include "types.h"
// func_0200e6d8 - find player actor by parameter
struct dActor_c {
    void* vtable;    // 0x0
    u32 uniqueID;    // 0x4
    u32 param1;      // 0x8
};

extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern u8 data_0209f250;
extern struct dActor_c* data_0209f394[];
extern u32 data_0209b284[];

struct dActor_c* func_0200e6d8(u32 arg0) {
    struct dActor_c* player;
    struct dActor_c* result;

    player = data_0209f394[data_0209f250];
    if (arg0 == player->param1) {
        result = player;
    } else {
        result = _ZN8dActor_c10FindWithIDEj(data_0209b284[arg0]);
    }
    return result;
}