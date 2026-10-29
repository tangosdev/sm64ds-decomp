//cpp
/**
 * Wet-Dry World's square floating board.
 *
 * InitResources loads the shared float-board model and collision, then
 * if the board spawned above the water it raycasts down onto the surface
 * (or the first hit under it) before remembering that rest pose.
 *
 * daObjWcObj01_c_classInit is reconstructed (RTTI daObjWcObj01_c,
 * WC_OBJ01 registry). Retail does not store that spelling.
 *
 * NOTES:
 * - The ray runs from 20.0 above the board down to the water height. The
 *   component stores follow the ROM's order, including the y stores that
 *   are overwritten at once.
 * - g_profile_WC_OBJ01 is ov029 data outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj01_AssetId { ASSET_WC_OBJ01_BMD = 0x06cc, ASSET_WC_OBJ01_KCL = 0x06cd };

#include "daObjWcObj01_c.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"

extern daObjFloatBoard_c_Resources data_ov029_02113be8;

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcObj01ModelFilePtr : SharedFilePtr {
    WcObj01ModelFilePtr(u32 fileID);
    ~WcObj01ModelFilePtr();
};

struct WcObj01CollisionFilePtr : SharedFilePtr {
    WcObj01CollisionFilePtr(u32 fileID);
    ~WcObj01CollisionFilePtr();
};

enum {
    kRayStartAboveFix12 = 0x14000 /* 20.0 */
};

// @symbol daObjWcObj01_c_classInit
extern "C" daObjWcObj01_c *daObjWcObj01_c_classInit() {
    return new daObjWcObj01_c();
}

// @symbol _ZN14daObjWcObj01_c13InitResourcesEv
int daObjWcObj01_c::InitResources() {
    Vector3 start, end;
    int waterY;
    int x, y, z;

    if (InitResourcesFrom(&data_ov029_02113be8) != 0) {
        waterY = GetWaterHeightWDW();
        if (mPosY > waterY) {
            dBgCh_Lin line;
            x = mPosX;
            end.x = x;
            y = mPosY;
            end.y = y;
            z = mPosZ;
            start.y = y;
            end.z = z;
            start.x = x;
            start.z = z;
            start.y = y + kRayStartAboveFix12;
            end.y = waterY;
            line.SetObjAndLine(start, end, this);
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

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj01.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj01ModelFilePtr gWcObj01ModelFile(ASSET_WC_OBJ01_BMD);
WcObj01CollisionFilePtr gWcObj01CollisionFile(ASSET_WC_OBJ01_KCL);
