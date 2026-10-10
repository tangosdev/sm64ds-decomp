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
 * deslop
 * Leftover: GetClsnPos is still the mangled call (Vector3-by-value
 *   return emits D1). startEnd is two Vector3 copies flattened
 *   (start = pos; end = pos; start.y += 20; end.y = waterY); the
 *   extra stores are decompiler dead stores, not an int[6] type.
 * Leftover: g_profile_WC_OBJ01 is ov029 data outside this TU (S14).
 */

#include "daObjWcObj01_c.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"

extern "C" daObjFloatBoard_c_Resources data_ov029_02113be8;
/* local extern: the header returns Vector3 by value, and the temporary emits a
   Vector3 destructor this TU does not own (rombuild isolate refuses it) */
extern "C" void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3 *out, dBgCh_Lin *self);

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcObj01ModelFilePtr : SharedFilePtr {
    u32 words[2];

    WcObj01ModelFilePtr(u32 fileID);
    ~WcObj01ModelFilePtr();
};

struct WcObj01CollisionFilePtr : SharedFilePtr {
    u32 words[2];

    WcObj01CollisionFilePtr(u32 fileID);
    ~WcObj01CollisionFilePtr();
};

enum {
    kRayStartAboveFix12 = 0x14000 /* 20.0 */
};

// @symbol daObjWcObj01_c_classInit
extern "C" daObjWcObj01_c *daObjWcObj01_c_classInit()
{
    return new daObjWcObj01_c();
}

// @symbol _ZN14daObjWcObj01_c13InitResourcesEv
int daObjWcObj01_c::InitResources()
{
    int startEnd[6]; /* start[3] then end[3] */
    int hit[3];
    int waterY;
    int x, y, z;

    if (func_ov002_020b5e58(&data_ov029_02113be8) != 0) {
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
                _ZN9dBgCh_Lin10GetClsnPosEv((Vector3 *)hit, &line);
                mPosY = hit[1];
            }
        }
        /* pad_320 lives in dBgActor_c tail padding at 0x31e, not here. */
        *(int *)((char *)this + 0x320) = mPosX;
        mWaterY = mPosY;
        *(int *)((char *)this + 0x328) = mPosZ;
        return 1;
    }
    return 0;
}

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj01.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj01ModelFilePtr data_ov029_02114220(0x6cc);
WcObj01CollisionFilePtr data_ov029_02114228(0x6cd);
