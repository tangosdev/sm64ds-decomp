//cpp
#include "TextureSequence.h"
// @symbol _ZN6Number13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Number.h"

struct SharedFilePtr { void* p; void* file; };
struct BMD_File;
struct BTP_File;

extern "C" {
    void _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr& f);
    void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
    int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
    void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int b, unsigned int c);
}

extern SharedFilePtr data_ov002_0210da08;
extern SharedFilePtr data_ov002_0210da28;
extern SharedFilePtr data_ov002_0210d9e8;
extern SharedFilePtr data_ov002_0210d9a8;

int Number::InitResources()
{
    if (param1 & 0x10) {
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210da08);
        void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da28);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, 1) == 0)
            return 0;
        TextureSequence::Prepare(*(BMD_File*)data_ov002_0210da28.file,
                                 *(BTP_File*)data_ov002_0210da08.file);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            ((char*)this) + 0x124, data_ov002_0210da08.file, 0x40000000, 0, 0);
        /* 0x12c is +0x8 inside the TextureSequence at 0x124 -- its Animation base's
           currFrame. The cartridge's own ~Number proves the extent; see
           tools/dtor_members.py. */
        mTextureSequence.currFrame = (int)((((unsigned int)(param1 & 0xf) % 10) << 16) >> 4);
    } else {
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210d9e8);
        void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a8);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, 1) == 0)
            return 0;
        TextureSequence::Prepare(*(BMD_File*)data_ov002_0210d9a8.file,
                                 *(BTP_File*)data_ov002_0210d9e8.file);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            ((char*)this) + 0x124, data_ov002_0210d9e8.file, 0x40000000, 0, 0);
        mTextureSequence.currFrame = (int)((((unsigned int)(param1 & 0xf) % 10) << 16) >> 4);
    }

    mState = 0;
    mStartPosX = mPosX;
    mStartPosY = mPosY;
    mStartPosZ = mPosZ;
    mVertSpeed = 0x14000;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    *(short*)(((char*)this) + 0x100 + 0x4c) = 0;
    mOwnerUniqueID = 0;
    mFollowOffsetY = 0;
    return 1;
}
