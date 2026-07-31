#include "types.h"
// func_0200e55c - find cutscene object by owner ID
struct Actor {
    void* vtable;    // 0x0
    u32 uniqueID;    // 0x4
    u32 param1;      // 0x8
};

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern u32 CUTSCENE_OBJECT_IDS[];

struct Actor* func_0200e55c(u32 ownerID) {
    u32* ids = CUTSCENE_OBJECT_IDS;
    int i = 0;
    struct Actor* a;
    do {
        a = _ZN5Actor10FindWithIDEj(*ids);
        if (a && ownerID == a->param1)
            return a;
        i++;
        ids++;
    } while (i < 0x10);
    return 0;
}