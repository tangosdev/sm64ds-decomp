//cpp
// @symbol _ZN32FloatOnWaterPlatformWdwRectangle13InitResourcesEv
#include "FloatOnWaterPlatformWdwRectangle.h"
#include "dBgCh_Lin.h"

/* FloatOnWaterPlatformWdwRectangle::InitResources -- vtable slot 0, ov029
 * 0x02111f58. Identical shape to FloatOnWaterPlatformWdwSquare::
 * InitResources (this overlay), just a different file table
 * (data_ov029_02113f00) -- see that file's note for the field/helper
 * rationale. */
extern "C" {
extern int func_ov002_020b5e58(void* c, void* d);
extern int _ZN8dActor_c17GetWaterHeightWDWEv(void* c);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3*, dBgCh_Lin*);
extern int data_ov029_02113f00[];
}

int FloatOnWaterPlatformWdwRectangle::InitResources()
{
    char *c = (char *)this;
    int sppad[6];
    int pos[3];
    int wh;
    int x, y, z;

    if (func_ov002_020b5e58(c, data_ov029_02113f00) != 0) {
        wh = _ZN8dActor_c17GetWaterHeightWDWEv(c);
        if (mPosY > wh) {
            dBgCh_Lin line;
            x = mPosX;
            sppad[3] = x;
            y = mPosY;
            sppad[4] = y;
            z = mPosZ;
            sppad[1] = y;
            sppad[5] = z;
            sppad[0] = x;
            sppad[2] = z;
            sppad[1] = y + 0x14000;
            sppad[4] = wh;
            line.SetObjAndLine(*(Vector3*)&sppad[0], *(Vector3*)&sppad[3], this);
            if (!line.DetectClsn()) {
                mPosY = wh;
            } else {
                _ZN9dBgCh_Lin10GetClsnPosEv((Vector3*)pos, &line);
                mPosY = pos[1];
            }
        }
        *(int*)(c + 0x320) = mPosX;
        mWaterY = mPosY;
        *(int*)(c + 0x328) = mPosZ;
        return 1;
    }
    return 0;
}
