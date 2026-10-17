extern int data_020874cc[];

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
    int mState_13c; /* 0x13c */
    char pad_140[0x3c];
    short mAngleY; /* 0x17c */
} CameraView;

int func_02005060(void* c){
    CameraView *cam = (CameraView *)c;
    cam->mState_13c = (int)data_020874cc;
    PlayerView *r0 = cam->mTargetPlayer;
    int r3 = cam->mAngleY;
    int r1 = r0->mAngleY;
    r1 = (short)(r1 + 0x8000);
    /* 0x184 stays raw: a yaw delta here and in func_0200bb28, role unshown. */
    *(short *)((char *)c + 0x100 + 0x84) = (short)(r1 - r3);
    return 1;
}
