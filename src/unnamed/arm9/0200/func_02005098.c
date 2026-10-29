extern int func_0200bec4(int a0, int a1, int a2, int a3, int a4);
extern int func_02009138(int a0, int a1);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    char pad_000[0x110];
    char *mTargetPlayer; /* 0x110 */
    char pad_114[0x28];
    int mMode; /* 0x13c */
} CameraView;

int func_02005098(int param_1) {
    CameraView *cam = (CameraView *)param_1;
    /* + 0x5c is the player's mPosX..mPosZ (include/dActor_c.h); + 0x644 is
       a Player field with no name yet. */
    char *ip = cam->mTargetPlayer;
    func_0200bec4(param_1, (int)(ip + 0x5c), *(int *)(ip + 0x644), cam->mMode, 0x20000);
    func_02009138(param_1, 0x80000);
    return 1;
}
