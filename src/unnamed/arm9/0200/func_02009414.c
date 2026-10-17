// @symbol func_02009414
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern short Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * helper touches. func_02008b4c walks a vector along a path, advancing the
 * shared segment index and the fixed-point progress within it. */
typedef struct {
    u8 pad_000[0x80];
    struct Vector3 lookAt; /* 0x080 */
    struct Vector3 pos;    /* 0x08c */
    u8 pad_098[0xb4];
    void *mLookAtPath; /* 0x14c -- set by func_020082a0 */
    void *mPosPath;    /* 0x150 -- set by func_020082a0 */
    u8 pad_154[0x18];
    s32 mPathProgress; /* 0x16c -- cleared by func_020094a8 */
    u8 pad_170[0xc];
    s16 mAngleY; /* 0x17c */
    s16 mAngleX; /* 0x17e */
    u8 pad_180[0x18];
    s16 mPathIndex; /* 0x198 -- cleared by func_020094a8 */
} CameraView;

int func_02009414(char *obj) {
    CameraView *cam = (CameraView *)obj;
    int r4 = 1;
    void *tbl1 = cam->mPosPath;

    if (tbl1 != 0) {
        r4 = func_02008b4c(&cam->pos, &cam->mPathIndex, &cam->mPathProgress, tbl1);
    }

    void *tbl2 = cam->mLookAtPath;
    if (tbl2 != 0) {
        if (func_02008b4c(&cam->lookAt, &cam->mPathIndex, &cam->mPathProgress, tbl2) == 0) {
            r4 = 0;
        }
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    cam->mAngleX = Vec3_VertAngle(&cam->lookAt, &cam->pos);

    func_020089f8(obj);
    return r4;
}
