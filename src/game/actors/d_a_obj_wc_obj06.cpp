//cpp
/**
 * Wet-Dry World's long floating board.
 *
 * InitResources loads the shared float-board model and collision, then
 * if the board spawned above the water it raycasts down onto the surface
 * (or the first hit under it) and writes that Y into mPos.y. It then
 * copies mPos.x/.y/.z onto mRestPosX, mWaterY, mRestPosZ.
 *
 * daObjWcObj06_c_classInit is reconstructed (RTTI daObjWcObj06_c,
 * WC_OBJ06 registry). Retail does not store that spelling.
 *
 * What stays:
 * - The ray's start and end are written field by field, in the ROM's store
 *   order (start = pos; end = pos; start.y += 20; end.y = waterY).
 * - g_profile_WC_OBJ06 is ov029 data outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj06_AssetId { ASSET_WC_OBJ06_BMD = 0x06d5, ASSET_WC_OBJ06_KCL = 0x06d6 };

#include "daObjWcObj06_c.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"

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

extern WcObj06ModelFilePtr gWcObj06ModelFile;
extern WcObj06CollisionFilePtr gWcObj06CollisionFile;

/* Collision properties: the 'CLPS' tag, the 8-byte stride header with two
 * entries, then the default entries. */
struct CLPS_Block {
    u32 tag;
    u32 header;
    u32 entries[6];
};
CLPS_Block data_ov029_0211308c = {
    0x53504c43,
    0x00020008,
    {0x01000fc0, 0x000000ff, 0x01000fc5, 0x000000ff, 0xffffffff, 0xffffffff},
};

daObjFloatBoard_c_Resources data_ov029_02113f00 = {
    &gWcObj06ModelFile,
    &gWcObj06CollisionFile,
    &data_ov029_0211308c,
};

enum {
    kRayStartAbove = 0x14000 /* 20.0 */
};

daObjWcObj06_c *daObjWcObj06_c_classInit() {
    return new daObjWcObj06_c();
}

int daObjWcObj06_c::InitResources() {
    Vector3 start, end;
    int waterY;
    int x, y, z;

    if (InitResourcesFrom(&data_ov029_02113f00) != 0) {
        waterY = GetWaterHeightWDW();
        if (mPos.y > waterY) {
            dBgCh_Lin line;
            x = mPos.x;
            end.x = x;
            y = mPos.y;
            end.y = y;
            z = mPos.z;
            start.y = y;
            end.z = z;
            start.x = x;
            start.z = z;
            start.y = y + kRayStartAbove;
            end.y = waterY;
            line.SetObjAndLine(start, end, this);
            if (line.DetectClsn() == 0) {
                mPos.y = waterY;
            } else {
                Vector3 hit = line.GetClsnPos();
                mPos.y = hit.y;
            }
        }
        mRestPosX = mPos.x;
        mWaterY = mPos.y;
        mRestPosZ = mPos.z;
        return 1;
    }
    return 0;
}

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj06.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj06ModelFilePtr gWcObj06ModelFile(ASSET_WC_OBJ06_BMD);
WcObj06CollisionFilePtr gWcObj06CollisionFile(ASSET_WC_OBJ06_KCL);
