//cpp
// @symbol _ZN13QuestionBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "QuestionBlock.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern void _ZN8dActor_c11UntrackStarERa(void *self, void *p);
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
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();

    b = (int)(actorID == 0x16);
    if (b)
        ((SharedFilePtr *)(data_ov002_0210da58))->Release();

    b2 = (int)(actorID == 0x14);
    if (b2)
        goto dosw;
    b3 = (int)(actorID == 0x15);
    if (b3) {
    dosw:
        switch (mContentType) {
        case 1:
            _ZN8dActor_c11UntrackStarERa(((char *)this), ((char *)this) + 0x3f0);
            break;
        case 3:
            ((SharedFilePtr *)(data_ov002_0210da18))->Release();
            break;
        case 2:
            ((SharedFilePtr *)(data_ov002_0210d9d8))->Release();
            break;
        case 4:
            ((SharedFilePtr *)(data_ov002_0210da30))->Release();
            break;
        case 7:
            ((SharedFilePtr *)(data_ov002_0210d9b0))->Release();
            ((SharedFilePtr *)(data_ov002_0210d9d0))->Release();
            ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
            break;
        case 5:
            ((SharedFilePtr *)(data_ov002_0210da58))->Release();
            ((SharedFilePtr *)(data_ov002_0210d9b0))->Release();
            ((SharedFilePtr *)(data_ov002_0210d9d0))->Release();
            break;
        case 6:
            ((SharedFilePtr *)(data_ov002_0210d9b0))->Release();
            ((SharedFilePtr *)(data_ov002_0210d9d0))->Release();
            break;
        }
    }

    switch (actorID - 0x14) {
    case 0:
        ((SharedFilePtr *)(data_ov102_0214e7e8))->Release();
        ((SharedFilePtr *)(data_ov102_0214e808))->Release();
        ((SharedFilePtr *)(data_ov102_0214e7f8))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
        break;
    case 1:
    case 2:
        ((SharedFilePtr *)(data_ov102_0214e800))->Release();
        break;
    case 3:
        ((SharedFilePtr *)(data_ov102_0214e7f0))->Release();
        ((SharedFilePtr *)(data_ov002_0210da40))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
        break;
    case 5:
        ((SharedFilePtr *)(data_ov102_0214e7d8))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9a0))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
        break;
    case 4:
        ((SharedFilePtr *)(data_ov102_0214e7e0))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9c0))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
    }
    ((SharedFilePtr *)(data_ov102_0214e7d0))->Release();
    return 1;
}
