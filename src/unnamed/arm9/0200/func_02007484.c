extern void _Z14ApproachLinearRiii(int *v, int target, int rate);
extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);

/* File-local views (the class headers are C++): only the fields this state
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
    unsigned char pad_000[0x8c];
    Vec3 pos; /* 0x08c */
    unsigned char pad_098[0x18];
    Vec3 savedLookAt; /* 0x0b0 */
    unsigned char pad_0bc[0x54];
    PlayerView *mTargetPlayer; /* 0x110 */
} CameraView;

int func_02007484(char *self)
{
    CameraView *cam = (CameraView *)self;
    _Z14ApproachLinearRiii(&cam->savedLookAt.z, 0, 0x2000);

    cam->savedLookAt.x = 0;

    Vec3 *target = &cam->mTargetPlayer->mPos;

    Math_Function_0203b0fc(&cam->pos.x, target->x, 0x28, 0x7fffffff);
    Math_Function_0203b0fc(&cam->pos.z, target->z, 0x28, 0x7fffffff);

    return 1;
}
