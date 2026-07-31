//cpp
// @symbol _ZN6Thwomp8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Thwomp.h"
extern "C" {
void func_ov091_02133020(char *c);
void func_ov091_02132ff4(char *c);
void func_ov091_02132f04(char *c);
void func_ov091_02132e98(char *c);
void func_ov091_02132e64(char *c);
void func_ov091_02133098(char *c);
int func_ov091_02132dc0(char *c);
int _ZN9Animation7AdvanceEv(void *self);
int _ZN9Animation8FinishedEv(void *self);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
}

int Thwomp::Behavior()
{
    if (mState < 2) {
        if (mTriggered != 0)
            mState = 2;
    }
    switch (mState) {
    case 0: {
        unsigned short cnt = (unsigned short)(unk_32c >> 12);
        if (cnt != 0) {
            unk_32c = (int)((((unsigned)cnt - 1) << 16) >> 4);
            cnt = (unsigned short)(unk_32c >> 12);
            if (cnt == 0) {
                char *p = ((char *)this) + 0x300;
                *(unsigned short *)(p + 0xa0) = 0xa;
            }
        } else {
            char *p = (char *)(((int)((char *)this) + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
            int t = *(unsigned short *)(p + 0xa0);
            if (t != 0) {
                unsigned short *q = (unsigned short *)(((int)((char *)this) + 0x3a0) & 0xFFFFFFFFFFFFFFFFLL);
                *q = *q - 1;
            } else {
                func_ov091_02133020(((char *)this));
            }
        }
        break;
    }
    case 1:
        func_ov091_02132ff4(((char *)this));
        break;
    case 2:
        if (mTriggered != 0) {
            unk_32c = 0;
            func_ov091_02132f04(((char *)this));
        } else {
            _ZN9Animation7AdvanceEv((char *)&mTextureSequence);
            if (_ZN9Animation8FinishedEv((char *)&mTextureSequence) != 0) {
                char *p = (char *)(((int)((char *)this) + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
                int t = *(unsigned short *)(p + 0xa0);
                if (t != 0) {
                    unsigned short *q = (unsigned short *)(((int)((char *)this) + 0x3a0) & 0xFFFFFFFFFFFFFFFFLL);
                    *q = *q - 1;
                } else {
                    func_ov091_02132f04(((char *)this));
                }
            } else {
                char *p = ((char *)this) + 0x300;
                *(unsigned short *)(p + 0xa0) = 5;
            }
        }
        break;
    case 3:
        func_ov091_02132e98(((char *)this));
        if (mState == 4) {
            if (mTriggered != 0) {
                unk_39e = 0x5a;
                mTriggered = 0;
            }
        }
        break;
    case 4:
        if (mTriggered != 0) {
            mTriggered = 0;
            unk_39e = 0x5a;
        }
        func_ov091_02132e64(((char *)this));
        break;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    func_ov091_02133098(((char *)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0) == 0) {
        if (func_ov091_02132dc0(((char *)this)) == 0)
            goto done;
    }
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
done:
    return 1;
}
