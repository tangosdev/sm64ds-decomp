//cpp
// @symbol _ZN9SeesawBob13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SeesawBob.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(int);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(int);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,short,int);
extern void func_020393d4(void*,void*);
extern void func_020393c4(void*,void*);
}

extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];

int SeesawBob::InitResources()
{
    unsigned char idx;
    int f;
    switch (unk_00c) {
        case 0x1c: ((char *)this)[0x31e] = 0; break;
        case 0x27: ((char *)this)[0x31e] = 1; break;
        case 0x85: ((char *)this)[0x31e] = 2; break;
        case 0x8f: ((char *)this)[0x31e] = 3; break;
        case 0x95: ((char *)this)[0x31e] = 4; break;
        case 0x96: ((char *)this)[0x31e] = 5; break;
        case 0x80: ((char *)this)[0x31e] = 6; break;
    }
    idx = (unsigned char)((char *)this)[0x31e];
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov095_021374a0 + idx*0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this)+0xd4, f, 1, -1);
    func_ov095_0213597c(((char *)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    {
        unsigned char i = (unsigned char)((char *)this)[0x31e];
        f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(int*)(data_ov095_021374a4 + i*0xc));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((char *)this)+0x124, f, ((char *)this)+0x2ec, 0x1000, unk_08e, *(int*)(data_ov095_021374a8 + i*0xc));
    }
    func_020393d4(((char *)this)+0x124, _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(((char *)this)+0x124, func_ov095_02135e90);
    return 1;
}
