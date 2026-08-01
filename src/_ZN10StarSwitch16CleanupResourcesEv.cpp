//cpp
// @symbol _ZN10StarSwitch16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "StarSwitch.h"
// _ZN10StarSwitch16CleanupResourcesEv at 0x020ba568
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void*);
void _ZN16MeshColliderBase7DisableEv(void*);
void _ZN13SharedFilePtr7ReleaseEv(void*);
void UnloadSilverStarAndNumber(void);
}

extern char data_ov002_021098e8;
extern char data_ov002_021098ec;
extern char data_ov002_0211092c;

int StarSwitch::CleanupResources()
{
    int t;
    if (_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(&data_ov002_021098e8 + unk_34c * 0xc));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(&data_ov002_021098ec + unk_34c * 0xc));
    t = mActorID == 0xc;
    if (t != false) {
        UnloadSilverStarAndNumber();
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211092c);
    }
    return 1;
}
