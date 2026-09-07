//cpp
#include "types.h"
// @symbol _ZN9FaderWipe11AdvanceFadeEv
/* recovered: named members + shared header, real C++ method */
#include "FaderWipe.h"
extern "C" {
void _ZN5Fader13AdvanceInterpEv(void* thiz);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16* p, u16 a, int b);
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToScale(void* m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(void* thiz, void* mtx, void* vec);
}
extern int data_020a0e68[];

void FaderWipe::AdvanceFade()
{
    Fix12i old = *(Fix12i*)((char*)&currInterp);
    Fix12i cur;
    _ZN5Fader13AdvanceInterpEv(((char*)this));
    cur = *(Fix12i*)((char*)&currInterp);
    if (cur == 0 && cur == old) return;
    if (cur == 0x1000) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4000050, 0x3f, -0x10);
    } else {
        if (*(Fix12i*)((char*)&speed) != 0) {
            if (old != 0x1000) *(u16*)0x4000050 = 0;
        }
        if (*(Fix12i*)((char*)&currInterp) != 0) {
            Fix12i scale = (Fix12i)(((long long)(0x1000 - *(Fix12i*)((char*)&currInterp)) * 0x20000 + 0x800) >> 12);
            Matrix4x3_FromTranslation(data_020a0e68, 0, 0, -0x1000);
            Matrix4x3_ApplyInPlaceToScale(data_020a0e68, scale, scale, scale);
            Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, 0x4000);
            _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(((char*)this) + 0x18, data_020a0e68, 0);
        }
    }
    if (*(Fix12i*)((char*)&currInterp) == old) return;
    {
        u16 color = this->color;
        int m = color ? 0x10 : -0x10;
        int prod = *(Fix12i*)((char*)&currInterp) * m;
        int r = prod >> 12;
        if (r != 0) {
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4001050, 0x3f, r);
        } else {
            *(u16*)0x4000050 = 0;
            *(u16*)0x4001050 = 0;
        }
    }
}
