#include "types.h"
// func_0200e55c - find cutscene object by owner ID
struct dActor_c {
    void* vtable;    // 0x0
    u32 uniqueID;    // 0x4
    u32 param1;      // 0x8
};

extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern u32 data_0209b2a4[];

struct dActor_c* func_0200e55c(u32 ownerID) {
    u32* ids = data_0209b2a4;
    int i = 0;
    struct dActor_c* a;
    do {
        a = _ZN8dActor_c10FindWithIDEj(*ids);
        if (a && ownerID == a->param1)
            return a;
        i++;
        ids++;
    } while (i < 0x10);
    return 0;
}