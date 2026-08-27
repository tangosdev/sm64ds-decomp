//cpp
// @symbol _ZN14QuestionSwitch13InitResourcesEv
#include "QuestionSwitch.h"
#include "SharedFilePtr.h"
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int b, int c, unsigned int d);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *f, void *m, int fix, short sh, void *b);
extern void func_020393c4(int *p, int v);
}

extern SharedFilePtr data_ov002_0210dd60;
extern SharedFilePtr data_ov002_0210dd68;
extern SharedFilePtr data_ov002_0210dd58;
extern SharedFilePtr data_ov002_0210dd50;
extern int data_ov002_0210d8b4;
extern int data_ov002_0210d774;
extern int data_0209caa0[];

int QuestionSwitch::InitResources()
{
    void *f;

    f = Model::LoadFile(data_ov002_0210dd60);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);

    f = Animation::LoadFile(data_ov002_0210dd68);
    /* Fix12<int> by value is a known mwccarm caller-side ABI wall here: the
       typed call grows this function by 0x14. Keep the ROM spelling until the
       shared Fix12 calling convention is solved. */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, f, 0, 0x1000, 0);
    mModelAnim.SetFlags(0x40000000);
    mModelAnim.speed = 0x1000;
    UpdateModelTransform();

    f = dBgW_Kc::LoadFile(data_ov002_0210dd58);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mStaticMeshCollider, f, &mClsnMat, 0x199, mAngleY,
        &data_ov002_0210d8b4);

    f = dBgW_Kc::LoadFile(data_ov002_0210dd50);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMovingMeshCollider, f, &mClsnMat, 0x199, mAngleY,
        &data_ov002_0210d774);

    func_020393c4((int *)&mStaticMeshCollider,
                  (int)&QuestionSwitch::AfterClsnCallback);

    if (data_0209caa0[1] & 0x80000000) {
        mActiveMeshCollider = &mMovingMeshCollider;
        mModelAnim.currFrame = mModelAnim.GetFrameCount() << 12;
        mModelAnim.Advance();
    } else {
        mActiveMeshCollider = &mStaticMeshCollider;
        mModelAnim.currFrame = 0;
    }

    UpdateClsnTransform();
    mPressTimer = 8;
    mTalkingPlayer = 0;
    return 1;
}
