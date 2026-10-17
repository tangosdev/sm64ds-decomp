struct dCamera_c;

extern int func_020091f8(struct dCamera_c *, struct dCamera_c *, int, int);
extern void Vec3_RotateYAndTranslate(void *out, int src, short angle,
                                    const int *data);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *dest, const void *src, int factor);
extern void func_02009138(struct dCamera_c *, int);
extern const int data_02086ea8[];

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * state touches. */
typedef struct {
    unsigned char pad_000[0x11c];
    /* LookAtExit points this at the actor's position ... */
    int mLookTarget; /* 0x11c */
    /* ... and copies that position here. */
    int mLookTargetPos[3]; /* 0x120 -- Vector3 */
    unsigned char pad_12c[0x5a];
    /* The yaw func_020091f8 eases mAngleY toward. */
    short mTargetAngleY; /* 0x186 */
} CameraView;

int func_02009540(struct dCamera_c *cam)
{
    CameraView *view = (CameraView *)cam;

    /* call func_020091f8(cam, &mLookTargetPos, mTargetAngleY, 0) */
    func_020091f8(cam, (struct dCamera_c *)view->mLookTargetPos, view->mTargetAngleY, 0);

    /* stack layout: 12-byte temporary vector, then three ints */
    unsigned char tmp_vec[12];
    int tmp_data[3];

    tmp_data[0] = data_02086ea8[0];
    tmp_data[1] = data_02086ea8[1];
    tmp_data[2] = data_02086ea8[2];

    Vec3_RotateYAndTranslate(tmp_vec, view->mLookTarget, view->mTargetAngleY, tmp_data);

    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((void *)view->mLookTargetPos, (const void *)tmp_vec,
                                             0x8000);

    /* func_02009138(cam, 0x300000) */
    func_02009138(cam, 0x300000);

    return 1;
}
