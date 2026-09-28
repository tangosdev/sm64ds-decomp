#ifndef DAOBJKM2_AMI_BOU_C_H
#define DAOBJKM2_AMI_BOU_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "dCcAc_c.h"

struct KCL_File;
struct CLPS_Block;

/* Scalar adapters for this class only. The real methods take Fix12<int>
 * by value and home those arguments; these int parameters do not.
 * Do not move the mangled names onto dBgW_KcMbg.h / dCcAc_c.h /
 * dBgActor_c.h. */
extern "C" {
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *act, int radius, int height,
    unsigned flags, unsigned vulnFlags);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(
    dBgActor_c *self, int rangeX, int rangeY);
}

/* Bowser in the Fire Sea net pole (profile KM2_AMI_BOU).
 * Direct base dBgActor_c. Factory allocates 0x358.
 * mdCcAc_c is the cylinder at 0x320. mHeightAng at 0x354 is the
 * sine-table phase: Behavior indexes this unsigned halfword, then
 * adds through a signed halfword. A plain u16 add loads ldrh where
 * the cartridge has ldrsh.
 */
struct daObjKm2_Ami_Bou_c : dBgActor_c {
    u8 pad_31e[0x2];
    dCcAc_c mdCcAc_c; /* 0x320 */
    u16 mHeightAng;   /* 0x354 */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daObjKm2_Ami_Bou_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();

    void SetMeshFile(KCL_File *kcl, int scale, CLPS_Block *clps) {
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, scale, mAngleY, clps);
    }
    void InitClsn(int radius, int height, unsigned flags, unsigned vulnFlags) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, radius, height, flags, vulnFlags);
    }
    int ClsnInRangeOnScreen(int rangeX, int rangeY) {
        return _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(
            this, rangeX, rangeY);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKm2_Ami_Bou_c_size_must_be_0x358[
    sizeof(daObjKm2_Ami_Bou_c) == 0x358 ? 1 : -1];
#endif

#endif /* DAOBJKM2_AMI_BOU_C_H */
