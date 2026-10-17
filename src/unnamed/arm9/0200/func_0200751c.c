struct Obj;

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
    unsigned char pad_000[0xb0];
    Vec3 savedLookAt; /* 0x0b0 */
    unsigned char pad_0bc[0x54];
    PlayerView *mTargetPlayer; /* 0x110 */
    unsigned char pad_114[0x72];
    /* The yaw func_020091f8 eases mAngleY toward; here, behind the player. */
    short mTargetAngleY; /* 0x186 */
} CameraView;

int func_0200751c(struct Obj *o) {
    CameraView *cam = (CameraView *)o;
    int tmp = cam->mTargetPlayer->mAngleY;
    tmp += 0x8000;
    cam->mTargetAngleY = (short)tmp;
    cam->savedLookAt.x = 0;
    cam->savedLookAt.y = 0;
    cam->savedLookAt.z = 0;
    return 1;
}
