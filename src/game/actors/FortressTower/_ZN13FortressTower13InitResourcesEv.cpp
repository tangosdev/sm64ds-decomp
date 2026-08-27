//cpp
// @symbol _ZN13FortressTower13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FortressTower.h"
extern "C" void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;

extern "C" int IsStarCollectedInCurLevel(int a);

extern void *data_ov102_0214e188[];
extern void *data_ov102_0214e18c[];
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

int FortressTower::InitResources()
{
    unsigned char *c = (unsigned char *)((void *)this);
    unsigned short id = *(unsigned short *)(c + 0xc);
    unsigned char idx;

    switch (id) {
    case 0x31:  *(unsigned char *)(c + 0x31e) = 2; break;
    case 0x3b:  *(unsigned char *)(c + 0x31e) = 1; break;
    case 0x3d:  *(unsigned char *)(c + 0x31e) = 0; break;
    case 0x12a: *(unsigned char *)(c + 0x31e) = 3; break;
    case 0x154: *(unsigned char *)(c + 0x31e) = 4; break;
    case 0x155: *(unsigned char *)(c + 0x31e) = 5; break;
    }

    idx = *(unsigned char *)(c + 0x31e);
    {
        void *f = _ZN5Model8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov102_0214e188 + idx * 0xc));
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);

    idx = *(unsigned char *)(c + 0x31e);
    if (idx == 0 || idx == 2) {
        void *f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov102_0214e18c + idx * 0xc));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, f, c + 0x2ec, 0x1000,
                                   *(short *)(c + 0x8e),
                                   *(void **)((char *)data_ov102_0214e190 + idx * 0xc));
    } else {
        void *f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void **)((char *)data_ov102_0214e18c + idx * 0xc));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, f, c + 0x2ec, 0x199,
                                   *(short *)(c + 0x8e),
                                   *(void **)((char *)data_ov102_0214e190 + idx * 0xc));
    }

    {
        int b31 = (int)(*(unsigned short *)(c + 0xc) == 0x31);
        if (b31 != 0 && data_0209f2f8 == 7) {
            if (data_0209f220 != 1) {
                if (IsStarCollectedInCurLevel(1) != 0)
                    goto ret1;
            }
            if (*(int *)(c + 0x60) >= 0xdac000)
                return 0;
        }
    }
ret1:
    return 1;
}
