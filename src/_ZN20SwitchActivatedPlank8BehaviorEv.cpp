//cpp
// @symbol _ZN20SwitchActivatedPlank8BehaviorEv
/* recovered: named members + shared header */
#include "SwitchActivatedPlank.h"
#include "dBgW.h"
extern "C" {
void func_020393a4(void* p, int v);
int _ZN5Event6GetBitEj(unsigned int);
void func_ov029_021126dc(char* c);
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void*, void*, int);

#pragma optimize_for_size on

int _ZN20SwitchActivatedPlank8BehaviorEv(struct SwitchActivatedPlank *self) {
    func_020393a4(((char*)self)+0x124, 0x100000);

    switch(self->mState){
    case 0:
        if(_ZN5Event6GetBitEj(self->mEventID) == 0) break;

        {
            unsigned char* st_ptr = (unsigned char*)(((int)((char*)self) + 0x3a2));
            *st_ptr = *st_ptr + 1;
        }

        *(short*)(((char*)self) + 0x300 + 0xa0) = 0;
        self->mVisible = 1;

        ((dBgW *)(((char*)self)+0x124))->Enable((dActor_c *)(((char*)self)));
        func_ov029_021126dc(((char*)self));
        _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(((char*)self)+0x124, ((char*)self)+0x370, self->mAngleY);
        break;

    case 1: {
        unsigned short* p = (unsigned short*)((char*)&self->mStateTimer);
        if (*(unsigned short*)(((char*)self) + 0x300 + 0xa0) > 0x168) {
            self->mVisible = *(unsigned short*)(((char*)self) + 0x300 + 0xa0) & 1;
        }
        *p = *p + 1;
        if (_ZN5Event6GetBitEj(self->mEventID) != 0) break;
        ((dBgW *)((char*)&self->mMovingMeshCollider))->Disable();
        self->mState = 0;
        self->mVisible = 0;
        break;
    }
    }

    return 1;
}
}
