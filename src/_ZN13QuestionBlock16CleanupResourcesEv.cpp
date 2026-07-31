//cpp
// @symbol _ZN13QuestionBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "QuestionBlock.h"
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *p);
extern void _ZN5Actor11UntrackStarERa(void *self, void *p);
}
extern char data_ov002_0210da58[];
extern char data_ov002_0210da18[];
extern char data_ov002_0210d9d8[];
extern char data_ov002_0210da30[];
extern char data_ov002_0210d9b0[];
extern char data_ov002_0210d9d0[];
extern char data_ov002_0210d9e0[];
extern char data_ov002_0210da40[];
extern char data_ov002_0210d9a0[];
extern char data_ov002_0210d9c0[];
extern char data_ov102_0214e7f8[];
extern char data_ov102_0214e7f0[];
extern char data_ov102_0214e7d0[];

int QuestionBlock::CleanupResources()
{
    int b, b2, b3;
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider))
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);

    b = (int)(mActorId == 0x16);
    if (b)
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da58);

    b2 = (int)(mActorId == 0x14);
    if (b2)
        goto dosw;
    b3 = (int)(mActorId == 0x15);
    if (b3) {
    dosw:
        switch (unk_3f3) {
        case 1:
            _ZN5Actor11UntrackStarERa(((char *)this), ((char *)this) + 0x3f0);
            break;
        case 3:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da18);
            break;
        case 2:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8);
            break;
        case 4:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da30);
            break;
        case 7:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
            break;
        case 5:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da58);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
            break;
        case 6:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
            break;
        }
    }

    switch (mActorId - 0x14) {
    case 0:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7e8);
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e808);
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7f8);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
        break;
    case 1:
    case 2:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e800);
        break;
    case 3:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7f0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da40);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
        break;
    case 5:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7d8);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
        break;
    case 4:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7e0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9c0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7d0);
    return 1;
}
