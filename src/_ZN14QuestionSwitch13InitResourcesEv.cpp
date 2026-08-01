//cpp
// @symbol _ZN14QuestionSwitch13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "QuestionSwitch.h"
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int b, int c);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int b, int c, unsigned int d);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *f, void *m, int fix, short sh, void *b);
extern void func_020393c4(int *p, int v);
extern void _ZN9Animation7AdvanceEv(void *self);

extern int data_ov002_0210dd60;
extern int data_ov002_0210dd68;
extern int data_ov002_0210dd58;
extern int data_ov002_0210dd50;
extern int data_0209caa0[];

int QuestionSwitch::InitResources()
{
    void *f;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210dd60);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x6b4, f, 1, -1);

    f = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210dd68);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0x6b4, f, 0, 0x1000, 0);
    _ZN9Animation8SetFlagsEi(((char *)this) + 0x704, 0x40000000);
    unk_710 = 0x1000;
    func_ov002_020b50a0(((char *)this));

    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov002_0210dd58);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char *)this) + 0x324, f, ((char *)this) + 0x2ec, 0x199, unk_08e, &data_ov002_0210d8b4);

    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov002_0210dd50);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char *)this) + 0x4ec, f, ((char *)this) + 0x2ec, 0x199, unk_08e, &data_ov002_0210d774);

    func_020393c4((int *)((char *)&unk_324), (int)&func_ov002_020b56c4);

    if (data_0209caa0[1] & 0x80000000) {
        mActiveMeshCollider = (int)((char *)&mMovingMeshCollider);
        unk_70c = _ZNK9Animation13GetFrameCountEv((char *)&mAnim) << 12;
        _ZN9Animation7AdvanceEv((char *)&mAnim);
    } else {
        mActiveMeshCollider = (int)((char *)&unk_324);
        unk_70c = 0;
    }

    func_ov002_020b503c(((char *)this));
    unk_718 = 8;
    mTalkingPlayer = 0;
    return 1;
}
