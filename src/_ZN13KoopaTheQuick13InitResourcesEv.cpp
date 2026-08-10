//cpp
// @symbol _ZN13KoopaTheQuick13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
/* recovered: named members + shared header, real C++ method */
#include "KoopaTheQuick.h"
typedef short s16;

extern "C" {
extern void _ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int n, int speed, unsigned int flags);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int r, int h, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int r, int h, void *p, int q);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, unsigned int idx);
extern unsigned char _ZN5Actor9TrackStarEjj(void *self, unsigned int a, unsigned int b);
}

extern char data_ov062_0211e00c[];
extern char data_ov062_0211e014[];
extern char data_ov062_0211e024[];
extern char data_ov062_0211e01c[];
extern char data_ov062_0211e034[];
extern char data_ov062_0211e03c[];
extern char data_ov062_0211e02c[];
extern char data_ov062_0211e004[];

int KoopaTheQuick::InitResources()
{
    unsigned char b;
    int zero;

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211e00c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e014);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e024);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e01c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e034);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e03c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e02c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e004);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii((char *)((void *)this) + 0x300, *(void **)(data_ov062_0211e00c + 4), 1, -1) == 0)
        return 0;
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)((void *)this) + 0x300, *(void **)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
    *(int *)((char *)&mScaleX) = 0x14cc;
    *(int *)((char *)&mScaleY) = 0x14cc;
    *(int *)((char *)&mScaleZ) = 0x14cc;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj((char *)((void *)this) + 0x110, ((void *)this), 0x78000, 0x12c000, 0x800004, 0);
    zero = 0;
    *(int *)((char *)&unk_38c) = zero;
    *(s16 *)((char *)&unk_3aa) = (s16)zero;
    *(unsigned char *)((char *)&unk_3b4) = (unsigned char)zero;
    *(int *)((char *)&unk_39c) = *(int *)((char *)&mPosX);
    *(int *)((char *)&unk_3a0) = *(int *)((char *)&mPosY);
    *(int *)((char *)&unk_3a4) = *(int *)((char *)&mPosZ);
    *(int *)((char *)&mVertAccel) = -0x2000;
    *(int *)((char *)&mTerminalVelocity) = -0x3c000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_((char *)((void *)this) + 0x144, ((void *)this), 0x78000, 0x78000, 0, 0);
    _ZN7PathPtr6FromIDEj((char *)((void *)this) + 0x3d8, *(unsigned int *)((char *)&param1) & 0xf);
    *(int *)((char *)&unk_3b8) = _ZNK7PathPtr8NumNodesEv((char *)&mPathPtr);
    *(int *)((char *)&unk_3bc) = zero;
    *(int *)((char *)&unk_3c0) = *(int *)((char *)&mPosX);
    *(int *)((char *)&unk_3c4) = *(int *)((char *)&mPosY);
    *(int *)((char *)&unk_3c8) = *(int *)((char *)&mPosZ);
    _ZNK7PathPtr7GetNodeER7Vector3j((char *)((void *)this) + 0x3d8, (char *)((void *)this) + 0x3cc, *(unsigned int *)((char *)&unk_3bc));
    *(unsigned char *)((char *)&unk_3ac) = (unsigned char)zero;
    b = (unsigned char)((((unsigned int)*(int *)((char *)&param1) >> 4) + 1) & 0x3f);
    *(unsigned char *)((char *)&unk_3ad) = b;
    b = (unsigned char)((((unsigned int)*(int *)((char *)&param1) >> 10) + 1) & 0x3f);
    *(unsigned char *)((char *)&unk_3ae) = b;
    if (*(unsigned char *)((char *)&unk_3ad) <= 1)
        *(unsigned char *)((char *)&unk_3ad) = 0xff;
    if (*(unsigned char *)((char *)&unk_3ae) <= 1)
        *(unsigned char *)((char *)&unk_3ae) = 0xff;
    *(unsigned char *)((char *)&unk_3b1) = (unsigned char)(*(s16 *)((char *)&mAngleX) & 0xf);
    b = *(unsigned char *)((char *)&unk_3b1);
    *(unsigned char *)((char *)&unk_3b0) = _ZN5Actor9TrackStarEjj(((void *)this), b, 2);
    *(int *)((char *)&unk_394) = zero;
    *(int *)((char *)&unk_398) = zero;
    *(unsigned char *)((char *)&unk_3b5) = (unsigned char)zero;
    *(unsigned char *)((char *)&unk_3b6) = (unsigned char)zero;
    return 1;
}
