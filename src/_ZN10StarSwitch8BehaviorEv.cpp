//cpp
// @symbol _ZN10StarSwitch8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "StarSwitch.h"
extern unsigned char IsAreaShowing(int idx);
extern void func_ov002_020ba01c(char *c, int mask, int b, int base, int target);
extern void func_ov002_020ba4d8(char *c, int i);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(char *c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int id, int vol);

extern int data_0209b454;

int StarSwitch::Behavior()
{
    void *a;
    int id344;
    int v;

    if (IsAreaShowing(unk_353) == 0) {
        mAreaId = unk_353;
        unk_338 = 1;
        func_ov002_020ba01c(((char *)this), 2, 1, 0x333, 0x1000);
        func_ov002_020ba4d8(((char *)this), 0);
        a = _ZN5Actor10FindWithIDEj(mTargetActorID);
        if (a != 0) {
            func_ov002_020e6d88(a);
        }
    }

    {
        int isType = (int)(mActorID == 0xc);
        if (isType != 0) {
            if (func_ov002_020b9f00(((char *)this)) != 0) {
                a = _ZN5Actor10FindWithIDEj(mTargetActorID);
                if (a != 0) {
                    *(void **)((char *)&unk_348) = a;
                }
            }
        }
    }

    if ((int)(mActorID == 0xc) != 0) {
        id344 = mTargetActorID;
        if (id344 == 0) {
            func_ov002_020ba01c(((char *)this), 2, 3, 0x333, 0x1000);
            _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
            if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0) != 0) {
                func_ov002_020b9f80(((char *)this));
            }
            return 1;
        }
        if (_ZN5Actor10FindWithIDEj(id344) == 0) {
            mTargetActorID = 0;
            mAreaId = unk_353;
            return 1;
        }
    }

    if ((data_0209b454 & 0x4000000) == 0) {
        unsigned short *p = (unsigned short *)(((long long)(int)((char *)&unk_338)));
        *p = *p + 1;
    }
    func_ov002_020ba520(((char *)this));
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0) != 0) {
        func_ov002_020b9f80(((char *)this));
    }

    if (unk_33c == 2) {
        if (unk_34d == 0) {
            v = unk_334;
            if (v != 0x40) {
                if (v == 0x7f) {
                    if ((data_0209b454 & 0x4000000) == 0) {
                        unk_34d = _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(v, 0x64cc);
                    }
                }
            } else {
                unk_34d = _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(v, 0xc999);
            }
        }
    }

    unk_350 = 0;
    return 1;
}
