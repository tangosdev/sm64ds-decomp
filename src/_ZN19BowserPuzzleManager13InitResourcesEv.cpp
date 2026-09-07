//cpp
// @symbol _ZN19BowserPuzzleManager13InitResourcesEv
#include "BowserPuzzleManager.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjFl_Coin_c.h"
// recovered name: daObjFl_Coin_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjFl_Coin_c::InitResources - byte-matched vs the ROM at mwccarm 2004/b56 with strict relocs (tools/match.py, size from ov064 symbols.txt); the vtable-slot-guess marker it carried was stale */
s32 BowserPuzzleManager::InitResources() {
    char * p = (char *)this;
    struct daObjFl_Coin_c *self = (struct daObjFl_Coin_c *)(void *)p;
    self->unk_0d5 = 0;
    self->unk_0d4 = 0;
    self->unk_0d6 = 0;
    return 1;
}
