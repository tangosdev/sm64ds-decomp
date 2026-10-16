extern int data_020874cc[];

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x8e];
    short mAngleY; /* 0x08e */
};

struct dCamera_c {
    char pad_000[0x110];
    struct dActor_c *mTargetPlayer; /* 0x110 */
    char pad_114[0x13c - 0x114];
    int mState_13c; /* 0x13c */
    char pad_140[0x17c - 0x140];
    short mAngleY; /* 0x17c */
    char pad_17e[0x184 - 0x17e];
    short mBaseAngleOffset; /* 0x184 */
};

int func_02005060(struct dCamera_c *c) {
    c->mState_13c = (int)data_020874cc;
    struct dActor_c *r0 = c->mTargetPlayer;
    int r3 = c->mAngleY;
    int r1 = r0->mAngleY;
    r1 = (short)(r1 + 0x8000);
    c->mBaseAngleOffset = (short)(r1 - r3);
    return 1;
}
