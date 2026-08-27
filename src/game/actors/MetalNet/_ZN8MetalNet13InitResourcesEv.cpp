//cpp
// @symbol _ZN8MetalNet13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "MetalNet.h"
typedef short s16;
struct SharedFilePtr { int x; }; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
unsigned char NumStars(void);
extern struct SharedFilePtr data_ov009_02113e90;
extern struct SharedFilePtr data_ov009_02113e88;
extern struct CLPS_Block data_ov009_02112bf8;
extern unsigned char data_0209f2d8;
extern int data_0209caa0;
}

int MetalNet::InitResources()
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov009_02113e90);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((unsigned char *)this) + 0xd4, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((unsigned char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((unsigned char *)this));
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov009_02113e88);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((unsigned char *)this) + 0x124, kcl, *(struct Matrix4x3 *)((unsigned char *)&(*(u8 *)&mClsnMat)), 0x1000,
            mAngleY, data_ov009_02112bf8);
    }
    if (((*(s32 *)&param1) & 0xff) == 0xff) {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) goto ret1;
        if ((*(int*)((char*)&data_0209caa0 + 8) & 0x80000) == 0) goto ret1;
        return 0;
    } else {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) goto ret1;
        if (NumStars() >= 0x96) return 0;
    }
ret1:
    return 1;
}
