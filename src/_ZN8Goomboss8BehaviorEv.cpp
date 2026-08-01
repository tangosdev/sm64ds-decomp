//cpp
// @symbol _ZN8Goomboss8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Goomboss.h"
typedef long long s64;

extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *thiz, void *clsn, unsigned int a);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void _ZN12CylinderClsn6UpdateEv(void *c);
extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(void *thiz, Vector3 *v, int b);
extern void _ZN5Actor13LandingDustAtER7Vector3b(void *thiz, Vector3 *v, int b);
extern void func_02012694(int a, void *p);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *thiz, Vector3 *v, int f);

extern char *data_0209f318;

int Goomboss::Behavior()
{
    int t;
    int u;
    int quake;

    if (mParam == 0x1111) {
        return func_ov074_021223bc(((char *)this));
    }

    t = ((s64)unk_5f6 * 0x646 + 0x800) >> 12;
    u = ((s64)t * 0x546000 + 0x800) >> 12;
    unk_5e8 = u < 0 ? -u : u;

    if (unk_5cc != 7) {
        *(void **)(data_0209f318 + 0x114) = ((char *)this);
    }

    func_ov074_0212199c(((char *)this));
    func_ov074_0212042c(((char *)this));
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char *)this), 0);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char *)this), ((char *)this) + 0x40c, 0);
    func_ov074_02120d74(((char *)this));
    func_ov074_02121380(((char *)this));

    {
        int i;
        char *p = ((char *)this) + 0x110;
        for (i = 0; i < 4; i++) {
            _ZN12CylinderClsn5ClearEv(p);
            _ZN12CylinderClsn6UpdateEv(p);
            p += 0x40;
        }
    }

    if (unk_5cc != 1 && unk_5cc != 0) {
        quake = data_ov074_0212290c[mSizeIndex];

        if (unk_5fe != 0) {
            if (mSizeIndex >= 2) {
                Vector3 v;
                v.x = unk_3c4;
                v.y = unk_3c8;
                v.z = unk_3cc;
                _ZN5Actor17HugeLandingDustAtER7Vector3b(((char *)this), &v, 1);
            } else {
                Vector3 v;
                v.x = unk_3c4;
                v.y = unk_3c8;
                v.z = unk_3cc;
                _ZN5Actor13LandingDustAtER7Vector3b(((char *)this), &v, 1);
            }
            func_02012694(0x15e, ((char *)this) + 0x74);
            {
                Vector3 v;
                v.x = unk_3c4;
                v.y = unk_3c8;
                v.z = unk_3cc;
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(((char *)this), &v, quake);
            }
        }

        if (unk_5ff != 0) {
            if (mSizeIndex >= 2) {
                Vector3 v;
                v.x = unk_3b8;
                v.y = unk_3bc;
                v.z = unk_3c0;
                _ZN5Actor17HugeLandingDustAtER7Vector3b(((char *)this), &v, 1);
            } else {
                Vector3 v;
                v.x = unk_3b8;
                v.y = unk_3bc;
                v.z = unk_3c0;
                _ZN5Actor13LandingDustAtER7Vector3b(((char *)this), &v, 1);
            }
            func_02012694(0x15e, ((char *)this) + 0x74);
            {
                Vector3 v;
                v.x = unk_3b8;
                v.y = unk_3bc;
                v.z = unk_3c0;
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(((char *)this), &v, quake);
            }
        }
    }

    func_ov074_02121300(((char *)this));
    func_ov074_0212195c(((char *)this));
    func_ov074_02121800(((char *)this));
    return 1;
}
