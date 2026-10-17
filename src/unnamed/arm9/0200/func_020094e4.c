extern void func_0200cb58(void *obj, int index);
extern void Vec3_RotateYAndTranslate(void *dst, void *src, short angle, const int *ptr);
extern int data_02086eb4[];

/* File-local views (the class headers are C++): only the fields this state
 * touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h. */
typedef struct {
    unsigned char pad_000[0x8e];
    short mAngleY; /* 0x08e */
} PlayerView;

typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    unsigned char pad_000[0x8c];
    Vec3 pos; /* 0x08c */
    /* func_020089f8 copies mTargetPlayer->mPos here every frame. */
    Vec3 mTargetPos; /* 0x098 */
    unsigned char pad_0a4[0x6c];
    PlayerView *mTargetPlayer; /* 0x110 */
} CameraView;

int func_020094e4(void *cam)
{
    CameraView *view = (CameraView *)cam;
    int buf[3];
    func_0200cb58(cam, 0x1f);
    buf[0] = data_02086eb4[0];
    buf[1] = data_02086eb4[1];
    buf[2] = data_02086eb4[2];
    short angle = view->mTargetPlayer->mAngleY;
    Vec3_RotateYAndTranslate(&view->pos, &view->mTargetPos, angle, buf);
    return 1;
}
