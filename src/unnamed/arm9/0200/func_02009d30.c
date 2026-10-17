extern int data_0208729c;

/* File-local views (the class headers are C++): only the fields this state
 * entry touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h. */
typedef struct {
    char pad_000[0x8e];
    short mAngleY; /* 0x08e */
} PlayerView;

typedef struct {
    char pad_000[0x110];
    PlayerView *mTargetPlayer; /* 0x110 */
    char pad_114[0x28];
    int *mState_13c; /* 0x13c */
    char pad_140[0x14];
    unsigned int mFlags; /* 0x154 */
    char pad_158[0x2e];
    /* The yaw func_020091f8 eases mAngleY toward; here, behind the player. */
    short mTargetAngleY; /* 0x186 */
} CameraView;

int func_02009d30(int *p)
{
    CameraView *cam = (CameraView *)p;
    int one = 1;

    cam->mState_13c = &data_0208729c;

    /* 0x1a6 (the header's unk_1a6) stays raw: set here on entering the state,
       cleared once func_020091f8 reports the camera has arrived. */
    *(unsigned char *)((char *)p + 0x1a6) = (unsigned char)one;

    cam->mTargetAngleY = (short)(cam->mTargetPlayer->mAngleY + 0x8000);
    cam->mFlags &= ~0x100u;

    return one;
}
