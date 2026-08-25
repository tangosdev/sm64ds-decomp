//cpp
// @symbol _ZN13QuestionBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "QuestionBlock.h"
#include "dBgW.h"
extern "C" {
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* clsn);
extern void func_020393a4(int* p, int v);
extern void func_02039394(int* p, int v);
extern void _ZN9Animation7AdvanceEv(void* a);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
}
extern int data_0209caa0[];
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;

int QuestionBlock::Behavior()
{
    func_ov102_02149df0(((char*)this));
    if (mState != 2) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
        if (mPosY <= mHomePosY) mPosY = mHomePosY;
        func_ov102_02149ff0(((char*)this));
        func_ov102_02149ea4(((char*)this));
    }
    func_020393a4((int*)&mMeshCollider, 0x8c000);
    func_02039394((int*)&mMeshCollider, 0x46000);
    if (mState != 0) {
        if (((dBgW *)&mMeshCollider)->IsEnabled() != 0) {
            ((dBgW *)&mMeshCollider)->Disable();
        }
        goto end;
    }
    if ((data_0209caa0[1] & 0x80000000) == 0) {
        int b = (int)(actorID == 0x14);
        if (b != 0) {
            _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
            if (((dBgW *)&mMeshCollider)->IsEnabled() != 0) {
                ((dBgW *)&mMeshCollider)->Disable();
            }
            goto end;
        }
    }
    {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) {
            if (((dBgW *)&mMeshCollider)->IsEnabled() == 0) {
                ((dBgW *)(&mMeshCollider))->Enable((dActor_c *)(((char*)this)));
            }
            goto end;
        }
    }
    if (data_0209f2f8 == 0x1c) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x460000, 0x46000) != 0) {
            func_ov102_02149e38(((char*)this));
        }
        goto end;
    }
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x118000, 0x46000) != 0) {
        func_ov102_02149e38(((char*)this));
    }
end:
    return 1;
}
