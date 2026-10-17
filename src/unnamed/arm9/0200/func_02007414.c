/* File-local views (the class headers are C++): only the fields this state
 * entry touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h. */
typedef struct {
    char pad_000[0x60];
    int mPosY; /* 0x060 */
} PlayerView;

typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    char pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
    char pad_08c[0x24];
    Vec3 savedLookAt; /* 0x0b0 */
    Vec3 savedPos;    /* 0x0bc */
    char pad_0c8[0x48];
    PlayerView *mTargetPlayer; /* 0x110 */
} CameraView;

int func_02007414(void *c)
{
    CameraView *cam = (CameraView *)c;

    /* zero some fields */
    cam->savedLookAt.x = 0;
    cam->savedLookAt.y = 0;
    cam->savedLookAt.z = 0;

    /* load pointers/values */
    PlayerView *p1 = cam->mTargetPlayer;
    int r2 = cam->lookAt.y;
    int r1 = p1->mPosY;

    /* store more zeros */
    cam->savedPos.x = 0;

    /* compute and store result */
    r1 = r2 - r1;
    cam->savedPos.y = r1;

    cam->savedPos.z = 0;
    /* Words 0x170 and 0x174 stay raw: per-state scalars with no one role. */
    *(int *)((char *)c + 0x170) = 0;
    *(int *)((char *)c + 0x174) = 0;

    return 1;
}
