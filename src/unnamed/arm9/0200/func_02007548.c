typedef struct { int x, y, z; } Vec3;
extern void func_02007d0c(void *self, Vec3 *src, int a2, int a3, int a4);

/* File-local views (the class headers are C++): only the fields this state
 * touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h (mPos spans its mPosX..mPosZ). */
typedef struct {
    char pad_000[0x5c];
    Vec3 mPos; /* 0x05c */
} PlayerView;

typedef struct {
    char pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
    char pad_08c[0x84];
    PlayerView *mTargetPlayer; /* 0x110 */
} CameraView;

int func_02007548(int p0)
{
    CameraView *cam = (CameraView *)p0;
    Vec3 *base = &cam->mTargetPlayer->mPos;
    int y0 = base->y + 0xffecd000;
    int tz = base->z;
    int tx = base->x;
    Vec3 src;
    src.y = 0x197000 + (y0 >> 1);
    src.x = tx;
    src.z = tz;
    func_02007d0c(&cam->lookAt, &src, 0x800, 0xccc, 0x800);
    return 1;
}
