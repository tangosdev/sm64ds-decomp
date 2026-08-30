//cpp
// func_ov002_020cfd84 at 0x020cfd84
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
#include "dBgCh_Lin.h"
extern "C" {
extern int _ZNK5dBgPi9GetClsnIDEv(void* self);
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);

int func_ov002_020cfd84(void* actor, void* a, void* b) {
    dBgCh_Lin line;
    dBgPi result;
    line.SetObjAndLine(*(Vector3*)a, *(Vector3*)b, (dActor_c*)actor);
    if (line.DetectClsn()) {
        result.surface = line.surface;
        result.triangleID = line.triangleID;
        result.flags = line.flags;
        result.clsnID = line.clsnID;
        result.unk_020 = line.unk_020;
        result.unk_024 = line.unk_024;
        if (_ZNK5dBgPi9GetClsnIDEv(&result) != -1) {
            void* act = _ZN8dActor_c10FindWithIDEj((unsigned int)_ZNK5dBgPi9GetClsnIDEv(&result));
            if (act != 0) {
                int t = (*(unsigned short*)((char*)act + 0xc) == 0x12a);
                if (t == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
}
