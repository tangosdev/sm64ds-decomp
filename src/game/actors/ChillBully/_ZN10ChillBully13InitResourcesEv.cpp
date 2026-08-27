//cpp
// @symbol _ZN10ChillBully13InitResourcesEv
/* recovered: real C++ method -- vtable slot 0, previously func_ov027_0211181c
 *
 * daOts_c leaves slot 0 pure virtual (the word in its vtable is a literal zero), so
 * every one of its three children has to supply this. ChillBully's points the shared
 * file table at its own list and then claims a star slot.
 */
#include "ChillBully.h"
#include "decl_common.h"

extern "C" {
extern int func_ov064_02116ec0(void *);
extern int _ZN8dActor_c9TrackStarEjj(void *, unsigned int, unsigned int);
}

int ChillBully::InitResources()
{
    mFileTable = (int)data_ov027_021138f4;
    int r = func_ov064_02116ec0((char *)this);
    mStarIdx = param1 & 0xf;
    mStarSlot = _ZN8dActor_c9TrackStarEjj((char *)this, mStarIdx, 2);
    return r;
}
