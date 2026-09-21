//cpp
#include "Model.h"
// @symbol _ZN19daObjBlockItemTag_c13InitResourcesEv
#include "daObjBlockItemTag_c.h"

extern "C" {
extern void LoadSilverStarAndNumber();
extern int data_ov002_0210d9d8;
extern int data_ov002_0210da30;
extern int data_ov002_0210da18;
}

int daObjBlockItemTag_c::InitResources()
{
    mStarID = param1 & 0x7f;
    if (mStarID == 0x7f)
        mStarID = 0;

    switch (actorID) {
    case 0x141:
        mActionIndex = 0;
        Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210d9d8);
        break;
    case 0x142:
        mActionIndex = 1;
        Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210da30);
        break;
    case 0x143:
        mActionIndex = 2;
        Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210da18);
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
