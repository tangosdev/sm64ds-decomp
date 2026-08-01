//cpp
// @symbol _ZN9WaterBomb8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterBomb.h"
extern "C" {
extern void func_ov098_0213b63c(char* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, void* b);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* a, void* b, unsigned int j);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* p);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
}

int WaterBomb::Behavior()
{
    if (func_ov098_0213b6e0(((char*)this))) {
        func_ov098_0213b584(((char*)this));
        _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
        return 1;
    }

    int idx = unk_3c4;
    char* ent = (char*)&data_ov098_0213c930[idx * 2];
    int adj = *(int*)(ent + 4);
    char* self = ((char*)this) + (adj >> 1);
    void* fn;
    if (adj & 1) {
        void* vt = *(void**)self;
        fn = *(void**)((char*)vt + *(int*)ent);
    } else {
        fn = *(void**)ent;
    }
    ((void (*)(char*))fn)(self);

    if (unk_3c8 != 0) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), ((char*)this) + 0x110);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char*)this), ((char*)this) + 0x144, 0);
        if (_ZNK12WithMeshClsn8IsOnWallEv((char*)&mWithMeshClsn)) {
            if (unk_3c8 == 1) {
                func_ov098_0213b63c(((char*)this));
            } else {
                _ZN9ActorBase18MarkForDestructionEv(((char*)this));
            }
            return 0;
        }
        func_ov098_0213b584(((char*)this));
        _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
        _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
    } else {
        DecIfAbove0_Short((unsigned short*)((char*)&unk_100));
    }
    return 1;
}
