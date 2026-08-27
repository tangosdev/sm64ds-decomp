//cpp
// @symbol _ZN29FloatOnWaterPlatformWdwSquare13InitResourcesEv
#include "FloatOnWaterPlatformWdwSquare.h"

/* FloatOnWaterPlatformWdwSquare::InitResources -- vtable slot 0, ov029
 * 0x02111254.
 *
 * Real member function: mWaterY is daObjFloatBoard_c's own named field
 * (include/daObjFloatBoard_c.h). func_ov002_020b5e58 is daObjFloatBoard_c's
 * shared file-load helper (out of this task's scope, kept under its
 * existing name -- same cross-overlay call FloatOnWaterPlatformJrb::
 * InitResources makes in ov016). dActor_c::GetWaterHeightWDW and the
 * dBgCh_Lin helpers are this class's own recovery, kept exactly as
 * found: dBgCh_Lin is a raw byte buffer (0x7c bytes matches its real
 * size but this task does not migrate it to a named type), and the
 * sppad[] staging array reproduces the ROM's own stack layout for the
 * Vector3-by-value SetObjAndLine call. mWaterY/mPosX/mPosY/mPosZ are read
 * by name where the header already provides them; 0x320 stays a raw
 * offset -- daObjFloatBoard_c.h documents it as UNOBSERVED padding, not a
 * field this class's own bytes confirm the name of. */
extern "C" {
extern int func_ov002_020b5e58(void* c, void* d);
extern int _ZN8dActor_c17GetWaterHeightWDWEv(void* c);
extern void _ZN9dBgCh_LinC1Ev(void*);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void*, void*, void*, void*);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void*);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
extern void _ZN9dBgCh_LinD1Ev(void*);
extern int data_ov029_02113be8[];
}

int FloatOnWaterPlatformWdwSquare::InitResources()
{
    char *c = (char *)this;
    int sppad[6]; /* a[3] + b[3] at low stack */
    int pos[3];
    char rl[0x7c];
    int wh;
    int x, y, z;

    if (func_ov002_020b5e58(c, data_ov029_02113be8) != 0) {
        wh = _ZN8dActor_c17GetWaterHeightWDWEv(c);
        if (mPosY > wh) {
            _ZN9dBgCh_LinC1Ev(rl);
            x = mPosX;
            sppad[3] = x; /* b.x */
            y = mPosY;
            sppad[4] = y; /* b.y temp */
            z = mPosZ;
            sppad[1] = y; /* a.y temp */
            sppad[5] = z; /* b.z */
            sppad[0] = x; /* a.x */
            sppad[2] = z; /* a.z */
            sppad[1] = y + 0x14000;
            sppad[4] = wh;
            _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &sppad[0], &sppad[3], c);
            if (_ZN9dBgCh_Lin10DetectClsnEv(rl) == 0) {
                mPosY = wh;
            } else {
                _ZN9dBgCh_Lin10GetClsnPosEv(pos, rl);
                mPosY = pos[1];
            }
            _ZN9dBgCh_LinD1Ev(rl);
        }
        *(int*)(c + 0x320) = mPosX;
        mWaterY = mPosY;
        *(int*)(c + 0x328) = mPosZ;
        return 1;
    }
    return 0;
}
