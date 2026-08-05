//cpp
#include "MeshColliderBase.h"
// func_ov002_020bbcb8 at 0x020bbcb8
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
void func_ov002_020bbd5c(void* c, int i);
}

extern "C" void func_ov002_020bbcb8(char* c)
{
    int flags = *(int*)(c + 0xb0);
    bool t;

    t = flags & 0x400;
    if (t != false) {
        func_ov002_020bbd5c(c, 3);
    } else {
        t = flags & 0x2000;
        if (t != false) {
            func_ov002_020bbd5c(c, 4);
        } else {
            t = flags & 0x100;
            if (t == false) {
                func_ov002_020bbd5c(c, 4);
            }
        }
    }

    if (((MeshColliderBase *)(c + 0x124))->IsEnabled()) {
        ((MeshColliderBase *)(c + 0x124))->Disable();
    }
}
