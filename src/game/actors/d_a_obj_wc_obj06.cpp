//cpp
/**
 * Wet-Dry World's long floating board.
 *
 * InitResources loads the shared float-board model and collision, then
 * if the board spawned above the water it raycasts down onto the surface
 * (or the first hit under it) and writes that Y into mPosY. It then
 * copies mPosX/Y/Z onto mRestPosX, mWaterY, mRestPosZ.
 *
 * daObjWcObj06_c_classInit is reconstructed (RTTI daObjWcObj06_c,
 * WC_OBJ06 registry). Retail does not store that spelling.
 *
 * What stays:
 * - startEnd is two Vector3 copies flattened into one int[6]
 *   (start = pos; end = pos; start.y += 20; end.y = waterY); the store
 *   order is the ROM's.
 * - g_profile_WC_OBJ06 is ov029 data outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj06_AssetId { ASSET_WC_OBJ06_BMD = 0x06d5, ASSET_WC_OBJ06_KCL = 0x06d6 };

#include "daObjWcObj06_c.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"

extern daObjFloatBoard_c_Resources data_ov029_02113f00;

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcObj06ModelFilePtr : SharedFilePtr {
    WcObj06ModelFilePtr(u32 fileID);
    ~WcObj06ModelFilePtr();
};

struct WcObj06CollisionFilePtr : SharedFilePtr {
    WcObj06CollisionFilePtr(u32 fileID);
    ~WcObj06CollisionFilePtr();
};

enum {
    kRayStartAboveFix12 = 0x14000 /* 20.0 */
};

// @symbol daObjWcObj06_c_classInit
extern "C" daObjWcObj06_c *daObjWcObj06_c_classInit() {
    return new daObjWcObj06_c();
}

// @symbol _ZN14daObjWcObj06_c13InitResourcesEv
int daObjWcObj06_c::InitResources() {
    int startEnd[6]; /* start[3] then end[3] */
    int waterY;
    int x, y, z;

    if (InitResourcesFrom(&data_ov029_02113f00) != 0) {
        waterY = GetWaterHeightWDW();
        if (mPosY > waterY) {
            dBgCh_Lin line;
            x = mPosX;
            startEnd[3] = x; /* end.x */
            y = mPosY;
            startEnd[4] = y; /* end.y temp */
            z = mPosZ;
            startEnd[1] = y; /* start.y temp */
            startEnd[5] = z; /* end.z */
            startEnd[0] = x; /* start.x */
            startEnd[2] = z; /* start.z */
            startEnd[1] = y + kRayStartAboveFix12;
            startEnd[4] = waterY;
            line.SetObjAndLine(*(Vector3 *)&startEnd[0], *(Vector3 *)&startEnd[3], this);
            if (line.DetectClsn() == 0) {
                mPosY = waterY;
            } else {
                Vector3 hit = line.GetClsnPos();
                mPosY = hit.y;
            }
        }
        mRestPosX = mPosX;
        mWaterY = mPosY;
        mRestPosZ = mPosZ;
        return 1;
    }
    return 0;
}

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj06.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj06ModelFilePtr gWcObj06ModelFile(ASSET_WC_OBJ06_BMD);
WcObj06CollisionFilePtr gWcObj06CollisionFile(ASSET_WC_OBJ06_KCL);
