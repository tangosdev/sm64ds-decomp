#include "types.h"
#include "nitro/fx/fx.h"
extern Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);
extern u16 data_0209b274;

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * script op touches. */
typedef struct {
    u8 pad_000[0xf8];
    s32 mAspect; /* 0x0f8 */
    u8 pad_0fc[0x10];
    u8 mViewport[4]; /* 0x10c -- x1, y1, x2, y2 */
} CameraView;

int func_020082f8(char* self, u8* src, int lo, int hi) {
    CameraView *cam = (CameraView *)self;
    Fix12i slope = _ZN4cstd4fdivEii((data_0209b274 - lo) << 12, (hi - lo) << 12);
    cam->mViewport[0] = (u8)(FX_MUL(((int)src[0] << 12), slope) >> 12);
    cam->mViewport[1] = (u8)(FX_MUL(((int)src[1] << 12), slope) >> 12);
    cam->mViewport[2] = (u8)((FX_MUL((((int)src[2] - 0xff) << 12), slope) >> 12) + 0xff);
    cam->mViewport[3] = (u8)((FX_MUL((((int)src[3] - 0xbf) << 12), slope) >> 12) + 0xbf);
    cam->mAspect = _ZN4cstd4fdivEii((((u8)cam->mViewport[2] - (u8)cam->mViewport[0]) + 1) << 12,
                                    (((u8)cam->mViewport[3] - (u8)cam->mViewport[1]) + 1) << 12);
    return 1;
}
