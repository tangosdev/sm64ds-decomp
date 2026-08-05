//cpp
// @symbol _ZN12FortressWall13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "FortressWall.h"
typedef int Fix12;
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *kcl, void *mtx, Fix12 f, short s, void *blk);
}
struct E12 { void *p; int pad[2]; };
extern struct E12 data_ov079_02128058[];
extern struct E12 data_ov079_0212805c[];
extern struct E12 data_ov079_02128060[];

int FortressWall::InitResources()
{
    int b = (int)(mActorID == 0x30);
    if (b != 0) {
        unk_31e = 0;
        unk_31f = (unsigned char)mParam;
        if (unk_31f == 0xff)
            unk_31f = 0;
    } else {
        unk_31e = 1;
    }
    {
        int idx = unk_31e;
        void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_02128058[idx].p);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, mdl, 1, -1);
        _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
        _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
        {
            int idx2 = unk_31e;
            void *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov079_0212805c[idx2].p);
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char *)this) + 0x124, kcl, ((char *)this) + 0x2ec, 0x199, mAngleY, data_ov079_02128060[idx2].p);
        }
    }
    return 1;
}
