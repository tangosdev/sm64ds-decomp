//cpp
// @symbol _ZN12EnemySpawner13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "EnemySpawner.h"
extern "C" {
extern unsigned int _ZN5Event6GetBitEj(unsigned int bit);
extern void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
}
extern int data_ov002_0210d9e0;

int EnemySpawner::InitResources()
{
    unk_0d4 = 0xce;
    unk_0dc = unk_090 & 0x1f;
    unk_0d8 = _ZN5Event6GetBitEj(unk_0dc);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
    return 1;
}
