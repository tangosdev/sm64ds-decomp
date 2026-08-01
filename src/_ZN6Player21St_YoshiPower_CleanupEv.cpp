//cpp
// @symbol _ZN6Player21St_YoshiPower_CleanupEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020d718c(char*);
extern void func_ov002_020d6790(char*);
}

int Player::St_YoshiPower_Cleanup()
{
    char* r2 = *(char**)((char*)&mObjInMouth);
    if (r2 != 0) {
        int t = (int)((*(int*)(r2+0xb0) & 0x20000) != 0);
        if (t != 0) {
            *(int*)(((int)r2 + 0xb0)) &= ~0xe0000;
            func_ov002_020d718c(((char*)this));
            return 1;
        }
        *(int*)(((int)r2 + 0xb0)) &= ~0x20000;
    }
    if (mUseAltBodyModel == 0) {
        char* q = *(char**)((char*)&mObjInMouth);
        if (q != 0) {
            int eq = (int)(*(unsigned short*)(q+0xc) == 0xbf);
            if (eq == 0) {
                func_ov002_020d6790(((char*)this));
                goto end;
            }
        }
        func_ov002_020d718c(((char*)this));
    } else {
        if (*(char**)((char*)&mObjInMouth) == 0 && unk_6f4 == 0)
            func_ov002_020d71a0(((char*)this));
    }
end:
    return 1;
}
