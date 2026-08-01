//cpp
#include "types.h"
// @symbol _ZN9CameraTag8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CameraTag.h"
struct Vec3 { Fix12i x, y, z; };

extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Vec3_RotateYAndTranslate(struct Vec3* out, void* m, s16 ang, struct Vec3* in);
extern u8 data_0209f250;
extern char* data_0209f394[];
extern char data_020a0ebc;

int CameraTag::Behavior()
{
    char* obj;
    struct Vec3 d;
    struct Vec3 r;
    int v;

    obj = data_0209f394[data_0209f250];
    Vec3_Sub(&d, (struct Vec3*)(obj + 0x5c), (struct Vec3*)((char*)&unk_05c));
    Vec3_RotateYAndTranslate(&r, &data_020a0ebc, mAngleY, &d);

    v = r.x;
    if (v < 0) v = -v;
    if (v < mScaleX) {
        if (r.y > -0x96000) {
            if (r.y < mScaleY) {
                int z = r.z;
                int az = (z < 0) ? -z : z;
                if (az > 0x64000 && az < 0x190000) {
                    int area = (z < 0) ? mAngleX : mAngleZ;
                    *(char*)(obj + 0xcc) = (char)area;
                    ChangeArea((char)area);
                }
            }
        }
    }
    return 1;
}
