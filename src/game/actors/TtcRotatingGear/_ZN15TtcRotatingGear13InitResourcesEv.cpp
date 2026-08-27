//cpp
// @symbol _ZN15TtcRotatingGear13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingGear.h"
/* Mangled ROM names need extern "C" in a .cpp file, or the compiler re-mangles them
   into phantoms no module defines. */
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern "C" void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void*, void*, void*, int, short, void*);

extern unsigned char data_0209f2c0[];
extern char data_ov065_0211c0d4[];
extern char data_ov065_0211c0d0[];

int TtcRotatingGear::InitResources()
{
    void* mdl;
    void* kcl;
    unsigned int ip;
    unsigned int b;
    mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d98c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, mdl, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov065_0211d97c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, kcl, ((char*)this) + 0x2ec, 0x199, mAngleY, func_021121b8);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mMoveDir = 0;
    ip = data_0209f2c0[0];
    b = mMoveDir;
    mMoveTimer = *(unsigned short*)(data_ov065_0211c0d4 + (ip << 4) + (b << 3));
    b = mMoveDir;
    mVertSpeed = *(int*)(data_ov065_0211c0d0 + (ip << 4) + (b << 3));
    mTerminalVelocity = -0x3c000;
    return 1;
}
