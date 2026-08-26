//cpp
// @symbol _ZN20SwitchActivatedPlank8BehaviorEv
/* recovered: named members + shared header. Real class form since the
   SwitchActivatedPlank header grew the real inheritance chain; the previous
   spelling defined extern "C" _ZN20SwitchActivatedPlank8BehaviorEv by hand,
   which let the compiler mangle nothing. */
#include "SwitchActivatedPlank.h"
#include "dBgW.h"

extern "C" {
void func_020393a4(void* p, int v);
int _ZN5Event6GetBitEj(unsigned int);
void func_ov029_021126dc(char* c);
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void*, void*, int);
}

#pragma optimize_for_size on

int SwitchActivatedPlank::Behavior() {
    func_020393a4(((char*)this)+0x124, 0x100000);

    switch(this->mState){
    case 0:
        if(_ZN5Event6GetBitEj(this->mEventID) == 0) break;

        {
            unsigned char* st_ptr = (unsigned char*)(((int)((char*)this) + 0x3a2));
            *st_ptr = *st_ptr + 1;
        }

        *(short*)(((char*)this) + 0x300 + 0xa0) = 0;
        this->mVisible = 1;

        ((dBgW *)(((char*)this)+0x124))->Enable((dActor_c *)(((char*)this)));
        func_ov029_021126dc(((char*)this));
        _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(((char*)this)+0x124, ((char*)this)+0x370, this->mAngleY);
        break;

    case 1: {
        unsigned short* p = (unsigned short*)((char*)&this->mStateTimer);
        if (*(unsigned short*)(((char*)this) + 0x300 + 0xa0) > 0x168) {
            this->mVisible = *(unsigned short*)(((char*)this) + 0x300 + 0xa0) & 1;
        }
        *p = *p + 1;
        if (_ZN5Event6GetBitEj(this->mEventID) != 0) break;
        ((dBgW *)((char*)&this->mMeshCollider))->Disable();
        this->mState = 0;
        this->mVisible = 0;
        break;
    }
    }

    return 1;
}
