//cpp
// @symbol _ZN6BobOmb8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * This file worked a `char *c` through raw offsets. BobOmb.h derives from Enemy
 * now, so every one of them is a named field -- including the three that were
 * inside the collision volume: 0x128, 0x130 and 0x134 are CylinderClsn's
 * flags, hitFlags and otherOwner (+0x18/+0x20/+0x24).
 */
#include "BobOmb.h"

extern "C" {
extern void func_ov102_0214ae1c(void*);
extern int func_ov102_0214ab1c(void*);
extern int func_ov102_0214aa18(void*);
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void*,void*,void*,unsigned int);
extern void _ZN5Enemy11UpdateDeathER12WithMeshClsn(void*,void*);
extern void func_ov102_0214b128(void*);
extern void func_ov102_0214b53c(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern void func_ov102_0214b03c(void*);
extern void *_ZN5Actor10FindWithIDEj(unsigned int);
extern void func_020ada40(void*,void*,void*,unsigned int);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*,void*);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void*,void*,unsigned int);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void*);
extern int SurfaceInfo_TestFlag0x20(int*);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void*);
extern void func_ov102_0214beb4(void*);
extern int func_ov102_0214b248(void*);
extern void func_ov102_0214b384(void*,int);
extern void func_ov102_0214ad40(void*);
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
}

int BobOmb::Behavior()
{
    int flag;
    int r0;
    void *other;

    if (unk_3f6 != 0) {
        func_ov102_0214ae1c(this);
        return 1;
    }
    if (func_ov102_0214ab1c(this)) {
        return 1;
    }
    if (func_ov102_0214aa18(this)) {
        return 1;
    }

    r0 = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(this, (char *)&mWithMeshClsn, (char *)&mModelAnim, 0);
    if (r0 != 0) {
        if (r0 == 2) {
            func_ov102_0214ae1c(this);
        }
        return 1;
    }

    if (mDeathState != 0) {
        _ZN5Enemy11UpdateDeathER12WithMeshClsn(this, (char *)&mWithMeshClsn);
        func_ov102_0214b128(this);
        flag = mFlags & 0x100;
        flag = flag != 0;
        if (flag != false) {
            mDeathState = 0;
        } else if (mDeathState != 0) {
            func_ov102_0214b53c(this);
            _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
            _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
            return 1;
        }
    }

    func_ov102_0214b03c(this);
    if (unk_3dc != 5) {
        if (mVertAccel != 0) {
            if ((*(int*)(&mCylinderClsn.hitFlags) & 0x10) != 0) {
                short v[3];
                other = _ZN5Actor10FindWithIDEj(*(unsigned int*)(&mCylinderClsn.otherOwner));
                v[0] = -0x2000;
                v[1] = 0;
                v[2] = 0;
                func_020ada40(this, v, other, 0x32000);
                _ZN5Actor9UpdatePosEP12CylinderClsn(this, (char *)&mCylinderClsn);
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(this, (char *)&mWithMeshClsn, 0);
                _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
                return 1;
            }

            _ZN5Actor9UpdatePosEP12CylinderClsn(this, (char *)&mCylinderClsn);
            if (data_0209f2f8 == 6 && data_0209f220 == 3) {
                if (mHorzSpeed == 0x5000) {
                    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(this, (char *)&mWithMeshClsn, 3);
                } else {
                    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(this, (char *)&mWithMeshClsn, 2);
                }
            } else {
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(this, (char *)&mWithMeshClsn, 2);
            }

            if (_ZNK12WithMeshClsn10IsOnGroundEv((char *)&mWithMeshClsn)) {
                if (SurfaceInfo_TestFlag0x20((int*)((char*)_ZNK12WithMeshClsn14GetFloorResultEv((char *)&mWithMeshClsn)+4))) {
                    func_ov102_0214ae1c(this);
                    return 1;
                }
                if (_ZNK12WithMeshClsn8IsOnWallEv((char *)&mWithMeshClsn) && unk_3dc == 0) {
                    func_ov102_0214beb4(this);
                }
            }
        }

        r0 = func_ov102_0214b248(this);
        if (r0 == 0) {
            return 0;
        }

        if (*(unsigned int*)(&mCylinderClsn.otherOwner) != 0) {
            if ((*(int*)(&mCylinderClsn.hitFlags) & 0x4000) != 0) {
                func_ov102_0214b384(this, 4);
            }
            if (unk_3dc == 4) {
                unsigned char b = unk_3f5;
                if (b == 2 || b == 3) {
                    other = _ZN5Actor10FindWithIDEj(*(unsigned int*)(&mCylinderClsn.otherOwner));
                    if (other != 0) {
                        int flag2 = *(unsigned short*)((char*)other+0xc);
                        flag2 = flag2 == 0xbd;
                        if (flag2 != false) {
                            func_ov102_0214b384(this, 2);
                            {
                                int *p128 = (int *)(((unsigned long long)(unsigned)(&mCylinderClsn.flags)) & 0xFFFFFFFFFFFFFFFFULL);
                                *p128 |= 0x4000;
                            }
                        }
                    }
                }
            }
        }

        func_ov102_0214b53c(this);
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
        func_ov102_0214ad40(this);
    }
    return 1;
}
