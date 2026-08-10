//cpp
// @symbol _ZN13QuestionBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "QuestionBlock.h"
#include "MeshColliderBase.h"
extern "C" {
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern void func_020393a4(int* p, int v);
extern void func_02039394(int* p, int v);
extern void _ZN9Animation7AdvanceEv(void* a);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
}
extern int data_0209caa0[];
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;

int QuestionBlock::Behavior()
{
    func_ov102_02149df0(((char*)this));
    if (unk_3e8 != 2) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), 0);
        if (mPosY <= unk_3e0) mPosY = unk_3e0;
        func_ov102_02149ff0(((char*)this));
        func_ov102_02149ea4(((char*)this));
    }
    func_020393a4((int*)((char*)&(*(u8 *)&mMeshCollider)), 0x8c000);
    func_02039394((int*)((char*)&(*(u8 *)&mMeshCollider)), 0x46000);
    if (unk_3e8 != 0) {
        if (((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled() != 0) {
            ((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->Disable();
        }
        goto end;
    }
    if ((data_0209caa0[1] & 0x80000000) == 0) {
        int b = (int)(actorID == 0x14);
        if (b != 0) {
            _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
            if (((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled() != 0) {
                ((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->Disable();
            }
            goto end;
        }
    }
    {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) {
            if (((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled() == 0) {
                ((MeshColliderBase *)(&mMeshCollider))->Enable((Actor *)(((char*)this)));
            }
            goto end;
        }
    }
    if (data_0209f2f8 == 0x1c) {
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x460000, 0x46000) != 0) {
            func_ov102_02149e38(((char*)this));
        }
        goto end;
    }
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x118000, 0x46000) != 0) {
        func_ov102_02149e38(((char*)this));
    }
end:
    return 1;
}
