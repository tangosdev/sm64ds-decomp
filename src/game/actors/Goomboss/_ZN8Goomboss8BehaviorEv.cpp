//cpp
// @symbol _ZN8Goomboss8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Goomboss.h"
typedef long long s64;

extern "C" {
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *clsn);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *thiz, void *clsn, unsigned int a);
extern void _ZN5dCc_c5ClearEv(void *c);
extern void _ZN5dCc_c6UpdateEv(void *c);
extern void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(void *thiz, Vector3 *v, int b);
extern void _ZN8dActor_c13LandingDustAtER7Vector3b(void *thiz, Vector3 *v, int b);
extern void func_02012694(int a, void *p);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *thiz, Vector3 *v, int f);
}

extern char *data_0209f318;

int Goomboss::Behavior()
{
    int t;
    int u;
    int quake;

    if (param1 == 0x1111) {
        return func_ov074_021223bc(((char *)this));
    }

    t = ((s64)mWalkSpeed * 0x646 + 0x800) >> 12;
    u = ((s64)t * 0x546000 + 0x800) >> 12;
    mGoombaTargetSpeed = u < 0 ? -u : u;

    if (mState != 7) {
        *(void **)(data_0209f318 + 0x114) = ((char *)this);
    }

    func_ov074_0212199c(((char *)this));
    func_ov074_0212042c(((char *)this));
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), 0);
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), &mWithMeshClsn, 0);
    func_ov074_02120d74(((char *)this));
    func_ov074_02121380(((char *)this));

    {
        int i;
        char *p = (char *)mdCc_cs;
        for (i = 0; i < 4; i++) {
            _ZN5dCc_c5ClearEv(p);
            _ZN5dCc_c6UpdateEv(p);
            p += 0x40;
        }
    }

    if (mState != 1 && mState != 0) {
        quake = data_ov074_0212290c[mSizeIndex];

        if (mLeftFootSteppedOnGround != 0) {
            if (mSizeIndex >= 2) {
                Vector3 v;
                v.x = mCylClsnPos[2].x;
                v.y = mCylClsnPos[2].y;
                v.z = mCylClsnPos[2].z;
                _ZN8dActor_c17HugeLandingDustAtER7Vector3b(((char *)this), &v, 1);
            } else {
                Vector3 v;
                v.x = mCylClsnPos[2].x;
                v.y = mCylClsnPos[2].y;
                v.z = mCylClsnPos[2].z;
                _ZN8dActor_c13LandingDustAtER7Vector3b(((char *)this), &v, 1);
            }
            func_02012694(0x15e, &mCamSpacePosX);
            {
                Vector3 v;
                v.x = mCylClsnPos[2].x;
                v.y = mCylClsnPos[2].y;
                v.z = mCylClsnPos[2].z;
                _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(((char *)this), &v, quake);
            }
        }

        if (mRightFootSteppedOnGround != 0) {
            if (mSizeIndex >= 2) {
                Vector3 v;
                v.x = mCylClsnPos[1].x;
                v.y = mCylClsnPos[1].y;
                v.z = mCylClsnPos[1].z;
                _ZN8dActor_c17HugeLandingDustAtER7Vector3b(((char *)this), &v, 1);
            } else {
                Vector3 v;
                v.x = mCylClsnPos[1].x;
                v.y = mCylClsnPos[1].y;
                v.z = mCylClsnPos[1].z;
                _ZN8dActor_c13LandingDustAtER7Vector3b(((char *)this), &v, 1);
            }
            func_02012694(0x15e, &mCamSpacePosX);
            {
                Vector3 v;
                v.x = mCylClsnPos[1].x;
                v.y = mCylClsnPos[1].y;
                v.z = mCylClsnPos[1].z;
                _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(((char *)this), &v, quake);
            }
        }
    }

    func_ov074_02121300(((char *)this));
    func_ov074_0212195c(((char *)this));
    func_ov074_02121800(((char *)this));
    return 1;
}
