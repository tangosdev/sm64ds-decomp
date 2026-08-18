//cpp
// @symbol _ZN5Whomp8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"
struct dActor_c; typedef int (dActor_c::*PMF)();
struct WithMeshClsn;
struct CylinderClsn;

extern "C" {
int _ZN8dActor_c13DistToCPlayerEv(dActor_c* self);
void func_ov079_02123f34(dActor_c* self);
void _ZN8dActor_c9UpdatePosEP12CylinderClsn(dActor_c* self, CylinderClsn* c);
int _ZN12dEnemyBase_c15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(dActor_c* self, WithMeshClsn* w, int a, short b, int c, int d, void* e);
void _ZN12dEnemyBase_c12UpdateWMClsnER12WithMeshClsnj(dActor_c* self, WithMeshClsn* w, unsigned int n);
void func_ov079_02124188(dActor_c* self);
int func_ov079_021243e0(char* c, int r4);
int func_ov079_02123a8c(dActor_c* self);
void func_ov079_02124008(dActor_c* self);
}

extern int data_0209f318;
extern PMF data_ov079_02128280[];

int Whomp::Behavior()
{
    char* c = (char*)((dActor_c*)this);

    if (*(unsigned char*)(c + 0x414) != 0 && *(int*)(c + 0x3b0) != 9) {
        if (_ZN8dActor_c13DistToCPlayerEv(((dActor_c*)this)) < 0x1770000) {
            *(int*)(*(int*)&data_0209f318 + 0x114) = (int)((dActor_c*)this);
        }
    }

    func_ov079_02123f34(((dActor_c*)this));
    _ZN8dActor_c9UpdatePosEP12CylinderClsn(((dActor_c*)this), 0);

    if (*(int*)(c + 0x98) != 0) {
        if (_ZN12dEnemyBase_c15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(((dActor_c*)this), (WithMeshClsn*)(c + 0x110), 0x3c000, (short)0x2888, 0, 0, (void*)0x32000)) {
            *(int*)(c + 0x5c) = *(int*)(c + 0x3d4);
            *(int*)(c + 0x60) = *(int*)(c + 0x3d8);
            *(int*)(c + 0x64) = *(int*)(c + 0x3dc);
        } else {
            *(int*)(c + 0x3d4) = *(int*)(c + 0x5c);
            *(int*)(c + 0x3d8) = *(int*)(c + 0x60);
            *(int*)(c + 0x3dc) = *(int*)(c + 0x64);
        }
    } else {
        *(int*)(c + 0x3d4) = *(int*)(c + 0x5c);
        *(int*)(c + 0x3d8) = *(int*)(c + 0x60);
        *(int*)(c + 0x3dc) = *(int*)(c + 0x64);
    }

    _ZN12dEnemyBase_c12UpdateWMClsnER12WithMeshClsnj(((dActor_c*)this), (WithMeshClsn*)(c + 0x110), 0);

    {
        int idx = *(int*)(c + 0x3b0);
        PMF* pmf = &data_ov079_02128280[idx];
        (((dActor_c*)this)->**pmf)();

        {
            unsigned short* ctr = (unsigned short*)(c + 0x100);
            *ctr = *ctr + 1;
            if (idx != *(int*)(c + 0x3b0)) {
                *ctr = 0;
                *(unsigned char*)(c + 0x40c) = 0;
            }
        }
    }

    func_ov079_02124188(((dActor_c*)this));

    if (func_ov079_021243e0((char*)((dActor_c*)this), 0) == 0 || func_ov079_02123a8c(((dActor_c*)this)) != 0) {
        func_ov079_02124008(((dActor_c*)this));
    }

    *(unsigned char*)(c + 0x403) = 0;
    return 1;
}
