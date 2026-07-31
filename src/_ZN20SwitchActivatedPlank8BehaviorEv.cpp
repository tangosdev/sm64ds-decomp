//cpp
// @symbol _ZN20SwitchActivatedPlank8BehaviorEv
/* recovered: named members + shared header */
#include "SwitchActivatedPlank.h"
extern "C" {
void func_020393a4(void* p, int v);
int _ZN5Event6GetBitEj(unsigned int);
int _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
void func_ov029_021126dc(char* c);
int _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void*, void*, int);
int _ZN16MeshColliderBase7DisableEv(void*);

#pragma optimize_for_size on

int _ZN20SwitchActivatedPlank8BehaviorEv(struct SwitchActivatedPlank *self) {
    func_020393a4(((char*)self)+0x124, 0x100000);

    switch(self->unk_3a2){
    case 0:
        if(_ZN5Event6GetBitEj(self->unk_3a4) == 0) break;

        {
            unsigned char* st_ptr = (unsigned char*)(((int)((char*)self) + 0x3a2) & 0xFFFFFFFFFFFFFFFF);
            *st_ptr = *st_ptr + 1;
        }

        *(short*)(((char*)self) + 0x300 + 0xa0) = 0;
        self->unk_3a3 = 1;

        _ZN16MeshColliderBase6EnableEP5Actor(((char*)self)+0x124, ((char*)self));
        func_ov029_021126dc(((char*)self));
        _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(((char*)self)+0x124, ((char*)self)+0x370, self->unk_08e);
        break;

    case 1: {
        unsigned short* p = (unsigned short*)((char*)&self->unk_3a0);
        if (*(unsigned short*)(((char*)self) + 0x300 + 0xa0) > 0x168) {
            self->unk_3a3 = *(unsigned short*)(((char*)self) + 0x300 + 0xa0) & 1;
        }
        *p = *p + 1;
        if (_ZN5Event6GetBitEj(self->unk_3a4) != 0) break;
        _ZN16MeshColliderBase7DisableEv((char*)&self->mMovingMeshCollider);
        self->unk_3a2 = 0;
        self->unk_3a3 = 0;
        break;
    }
    }

    return 1;
}
}
