//cpp
// @symbol _ZN18BowserFireSeaArena8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFireSeaArena.h"

int BowserFireSeaArena::Behavior()
{
    short *a = (short *)(((int)((char *)this) + 0x8c) & 0xFFFFFFFFFFFFFFFF);
    short *b = (short *)(((int)((char *)this) + 0x8e) & 0xFFFFFFFFFFFFFFFF);

    *a = *a + unk_31e;
    *b = *b + unk_320;

    {
        short *d = (short *)(((int)((char *)this) + 0x90) & 0xFFFFFFFFFFFFFFFF);
        *d = *d + unk_322;
    }

    func_ov060_02117ae0(((char *)this));
    func_ov060_02117a64(((char *)this));
    return 1;
}
