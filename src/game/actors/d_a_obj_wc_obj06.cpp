//cpp
/**
 * Wet-Dry World's long floating board.
 *
 * InitResources loads the shared float-board model and collision, then
 * if the board spawned above the water it raycasts down onto the surface
 * (or the first hit under it) and writes that Y into mPosY. It then
 * copies mPosX/Y/Z onto unk_320, mWaterY, unk_328.
 *
 * daObjWcObj06_c_classInit is reconstructed (RTTI daObjWcObj06_c,
 * WC_OBJ06 registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b5e58 is still the linker name of
 *   daObjFloatBoard_c Init (slot 0 is pure virtual on the base).
 *   Naming belongs in ov002. That helper loads **(SharedFilePtr**)fp,
 *   fp+4, fp+8 as model/KCL/CLPS.
 * Leftover: GetClsnPos is still the mangled call (Vector3-by-value
 *   return emits D1). startEnd is two Vector3 copies flattened
 *   (start = pos; end = pos; start.y += 20; end.y = waterY); the
 *   extra stores are decompiler dead stores, not an int[6] type.
 * Leftover: g_profile_WC_OBJ06 is ov029 data outside this TU (S14).
 */

#include "daObjWcObj06_c.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" int func_ov002_020b5e58(void *self, ResourceDescriptor *data);
extern "C" ResourceDescriptor data_ov029_02113f00;
extern "C" void _ZN9dBgCh_Lin10GetClsnPosEv(s32 *out, dBgCh_Lin *self);

enum {
    kRayStartAboveFix12 = 0x14000 /* 20.0 */
};

// @symbol daObjWcObj06_c_classInit
extern "C" daObjWcObj06_c *daObjWcObj06_c_classInit()
{
    return new daObjWcObj06_c();
}

// @symbol _ZN14daObjWcObj06_c13InitResourcesEv
int daObjWcObj06_c::InitResources()
{
    int startEnd[6]; /* start[3] then end[3] */
    int hit[3];
    int waterY;
    int x, y, z;

    if (func_ov002_020b5e58(this, &data_ov029_02113f00) != 0) {
        waterY = GetWaterHeightWDW();
        if (mPosY > waterY) {
            dBgCh_Lin line;
            x = mPosX;
            startEnd[3] = x;                 /* end.x */
            y = mPosY;
            startEnd[4] = y;                 /* end.y temp */
            z = mPosZ;
            startEnd[1] = y;                 /* start.y temp */
            startEnd[5] = z;                 /* end.z */
            startEnd[0] = x;                 /* start.x */
            startEnd[2] = z;                 /* start.z */
            startEnd[1] = y + kRayStartAboveFix12;
            startEnd[4] = waterY;
            line.SetObjAndLine(*(Vector3 *)&startEnd[0],
                *(Vector3 *)&startEnd[3], this);
            if (line.DetectClsn() == 0) {
                mPosY = waterY;
            } else {
                _ZN9dBgCh_Lin10GetClsnPosEv(hit, &line);
                mPosY = hit[1];
            }
        }
        unk_320 = mPosX;
        mWaterY = mPosY;
        unk_328 = mPosZ;
        return 1;
    }
    return 0;
}
