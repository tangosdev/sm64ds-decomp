//cpp
// @symbol _ZN10BrickBlock13InitResourcesEv
#include "BrickBlock.h"

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *file);
extern void LoadSilverStarAndNumber();
extern int data_ov002_0210d9d8;
extern int data_ov002_0210da30;
extern int data_ov002_0210da18;
}

int BrickBlock::InitResources()
{
    mStarID = param1 & 0x7f;
    if (mStarID == 0x7f)
        mStarID = 0;

    switch (actorID) {
    case 0x141:
        mActionIndex = 0;
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d8);
        break;
    case 0x142:
        mActionIndex = 1;
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
        break;
    case 0x143:
        mActionIndex = 2;
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da18);
        break;
    case 0x144:
        mActionIndex = 3;
        /* TrackStar's star index is unsigned even though this stored byte is
         * signed at the other ROM load sites. The cast preserves the ldrb. */
        mTrackStarID = TrackStar((u8)mStarID, 1);
        LoadSilverStarAndNumber();
        break;
    }
    return 1;
}
