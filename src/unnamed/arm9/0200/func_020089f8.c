extern void Vec3_Sub(int *dest, const int *src1, const int *src2);

/* File-local views (the class headers are C++): only the fields this helper
 * touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h (mPos spans its mPosX..mPosZ). */
typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    unsigned char pad_000[0x5c];
    Vec3 mPos; /* 0x05c */
} PlayerView;

typedef struct {
    unsigned char pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
    unsigned char pad_08c[0xc];
    /* The target player's position, refreshed here every frame ... */
    Vec3 mTargetPos; /* 0x098 */
    /* ... and lookAt relative to it. */
    Vec3 mLookAtOffset; /* 0x0a4 */
    unsigned char pad_0b0[0x60];
    PlayerView *mTargetPlayer; /* 0x110 */
} CameraView;

void func_020089f8(int *obj)
{
    CameraView *cam = (CameraView *)obj;
    PlayerView *player;
    Vec3 *p;
    int tmp[3];

    player = cam->mTargetPlayer;
    if (player == 0) {
        return;
    }
    p = &player->mPos;
    cam->mTargetPos.x = p->x;
    cam->mTargetPos.y = p->y;
    cam->mTargetPos.z = p->z;
    Vec3_Sub(tmp, &cam->lookAt.x, &cam->mTargetPos.x);
    cam->mLookAtOffset.x = tmp[0];
    cam->mLookAtOffset.y = tmp[1];
    cam->mLookAtOffset.z = tmp[2];
    /* 0x130 stays raw: zeroed here, read once as a scale on mLookTargetPos. */
    obj[0x130 / 4] = 0;
}
