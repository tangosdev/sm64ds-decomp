struct dCamera_c;

extern void Vec3_RotateYAndTranslate(void *dst, void *src, short angle, void *unk);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    unsigned char pad_000[0x8c];
    Vec3 pos; /* 0x08c */
    unsigned char pad_098[0x18];
    Vec3 savedLookAt; /* 0x0b0 */
    unsigned char pad_0bc[0xca];
    /* The yaw func_020091f8 eases mAngleY toward. */
    short mTargetAngleY; /* 0x186 */
} CameraView;

int func_02007450(struct dCamera_c *cam)
{
    CameraView *view = (CameraView *)cam;
    short angle = view->mTargetAngleY;
    Vec3_RotateYAndTranslate(&view->pos, &view->pos, angle, &view->savedLookAt);
    return 1;
}
