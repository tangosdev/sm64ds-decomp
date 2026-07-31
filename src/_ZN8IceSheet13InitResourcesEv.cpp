//cpp
// @symbol _ZN8IceSheet13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "IceSheet.h"
typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);

int IceSheet::InitResources()
{
    void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov018_02113c84);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, mdl, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    void *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov018_02113c7c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char *)this) + 0x124, kcl, ((char *)this) + 0x2ec, 0x199, unk_08e, (void *)&data_ov002_0210d754);
    return 1;
}
