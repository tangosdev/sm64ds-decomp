typedef struct Vector3 Vector3;
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int SublevelToLevel(int i);
extern void func_020071a8(void *cam, short ang, int threshold);
extern signed char data_0209f2f8;
extern unsigned char data_0209f224;

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * state touches. */
typedef struct {
    unsigned char pad_000[0x8c];
    int pos[3]; /* 0x08c -- Vector3 */
    unsigned char pad_098[0xb0];
    Vector3 *mFixedViewPos; /* 0x148 */
    unsigned char pad_14c[0x56];
    /* func_020079ac's accumulated angle offsets, cleared here. */
    short mPitchOffset; /* 0x1a2 */
    short mYawOffset;   /* 0x1a4 */
} CameraView;

int func_02007d58(char *cam) {
    CameraView *view = (CameraView *)cam;
    int lvl;
    void *p;
    /* 0x19a stays raw: its writers store yaws, but no reader shows its role. */
    *(short*)(cam + 0x100 + 0x9a) = 0;
    view->mPitchOffset = 0;
    view->mYawOffset = 0;
    p = view->mFixedViewPos;
    if (p != 0) {
        *(short *)(cam + 0x100 + 0x9a) =
            Vec3_HorzAngle((const Vector3 *)p, (const Vector3 *)view->pos);
    }
    lvl = SublevelToLevel(data_0209f2f8);
    if (lvl == 3 && data_0209f224 == 4) {
        func_020071a8(cam, 0x5600, 0x800);
    } else if (lvl == 0xb && data_0209f224 == 2) {
        func_020071a8(cam, 0, 0x800);
    } else if (lvl == 9 && data_0209f224 == 1) {
        func_020071a8(cam, 0x2000, 0x800);
    } else if (lvl == 0xe && data_0209f224 == 3) {
        func_020071a8(cam, 0, 0x800);
    }
    return 1;
}
