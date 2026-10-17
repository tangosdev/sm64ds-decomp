//cpp
struct C;
typedef void (C::*PMF)();

/* The scene's three pointer-to-member steps (dScEntry_c's mEnterStep,
 * mBehaviorStep and mRenderStep at 0x54..0x6c). This file writes each as
 * its two words. */
struct StepSet {
    char pad_00[0x54];
    int mEnterStep[2];    /* 0x54 -- run as soon as a new set is installed */
    int mBehaviorStep[2]; /* 0x5c */
    int mRenderStep[2];   /* 0x64 */
};

extern "C" void func_ov075_0211a194(char* self, int* src)
{
    StepSet *s = (StepSet *)self;
    int a0 = src[0];
    int a1 = src[1];
    a1 = a1 ? a1 : a1;
    s->mEnterStep[0] = a0;
    s->mEnterStep[1] = a1;
    int a2 = src[2];
    int a3 = src[3];
    a3 = a3 ? a3 : a3;
    s->mBehaviorStep[0] = a2;
    s->mBehaviorStep[1] = a3;
    int a4 = src[4];
    int a5 = src[5];
    a5 = a5 ? a5 : a5;
    s->mRenderStep[0] = a4;
    s->mRenderStep[1] = a5;
    if (s->mEnterStep[0] != 0) {
        PMF *p = (PMF *)s->mEnterStep;
        C* o = (C*)self;
        (o->**p)();
    }
}
