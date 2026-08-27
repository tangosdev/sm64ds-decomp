//cpp
// @symbol _ZN13OneUpMushroom13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "OneUpMushroom.h"
#include "SharedFilePtr.h"
typedef struct BMD_File BMD_File;
typedef struct dActor_c dActor_c;

struct ModelCache { int pad0; BMD_File* file; };

extern ModelCache data_ov002_0210d9b8;
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov002_0210da30;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, int q);
extern "C" int IsStarCollectedInCurLevel(int a);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void* self);

int OneUpMushroom::InitResources()
{
    BMD_File* f;
    int isKind0, isKind115;

    mMushroomType = param1 & 0xf;

    isKind0 = (actorID == 0x114);
    if (isKind0) {
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x300, data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d8);
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x300, f, 1, 1) == 0)
                return 0;
        }
    } else {
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x300, data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x300, f, 1, 1) == 0)
                return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;

    if (mMushroomType == 6 || mMushroomType == 8 || (unsigned int)(mMushroomType - 0xb) <= 1) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, (dActor_c*)((char*)this), 0x64000, 0x40000, 0x100002, 0);
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            *(int*)((long long)((char*)&mdCcAc_c.vulnFlags)) |= 0x8000;
        }
    } else {
        isKind115 = (actorID == 0x115);
        if (isKind115) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, (dActor_c*)((char*)this), 0x41000, 0x41000, 0x100002, 0);
        } else {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, (dActor_c*)((char*)this), 0x32000, 0x32000, 0x100002, 0);
        }
    }

    unk_388 = 0;
    if (data_ov002_020ff040[mMushroomType] == 0) {
        *(int*)((long long)((char*)&mdCcAc_c.flags)) |= 1;
    }
    if (data_ov002_020ff050[mMushroomType] == 0) {
        *(int*)((long long)((char*)&mFlags)) &= ~1;
    }
    if ((unsigned int)(mMushroomType - 0xb) <= 1) {
        unk_38e = 1;
    } else {
        unk_38e = 0;
    }
    unk_38f = 1;
    unk_390 = ((unsigned int)param1 >> 4) & 0xf;
    unk_378 = mPosX;
    unk_37c = mPosY;
    unk_380 = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x144, (dActor_c*)((char*)this), 0x32000, 0x32000, 0, 0);
    _ZN10dBgCh_Actr13SetLimMovFlagEv((char*)&mWithMeshClsn);
    unk_394 = 0;

    if (data_0209f2f8 == 7 && mPosY == 0xdac000 && mPosZ == 0
        && (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0)) {
        _ZN7fBase_c18MarkForDestructionEv(((char*)this));
        return 0;
    }

    return 1;
}
