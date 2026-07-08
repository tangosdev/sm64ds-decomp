typedef unsigned short u16;
typedef int Fix12i;

void _ZN5Fader13AdvanceInterpEv(void* thiz);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16* p, u16 a, int b);
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToScale(void* m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(void* thiz, void* mtx, void* vec);
extern int MATRIX_SCRATCH_PAPER[];

void _ZN9FaderWipe11AdvanceFadeEv(char* thiz)
{
    Fix12i old = *(Fix12i*)(thiz + 4);
    Fix12i cur;
    _ZN5Fader13AdvanceInterpEv(thiz);
    cur = *(Fix12i*)(thiz + 4);
    if (cur == 0 && cur == old) return;
    if (cur == 0x1000) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4000050, 0x3f, -0x10);
    } else {
        if (*(Fix12i*)(thiz + 8) != 0) {
            if (old != 0x1000) *(u16*)0x4000050 = 0;
        }
        if (*(Fix12i*)(thiz + 4) != 0) {
            Fix12i scale = (Fix12i)(((long long)(0x1000 - *(Fix12i*)(thiz + 4)) * 0x20000 + 0x800) >> 12);
            Matrix4x3_FromTranslation(MATRIX_SCRATCH_PAPER, 0, 0, -0x1000);
            Matrix4x3_ApplyInPlaceToScale(MATRIX_SCRATCH_PAPER, scale, scale, scale);
            Matrix4x3_ApplyInPlaceToRotationX(MATRIX_SCRATCH_PAPER, 0x4000);
            _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(thiz + 0x18, MATRIX_SCRATCH_PAPER, 0);
        }
    }
    if (*(Fix12i*)(thiz + 4) == old) return;
    {
        u16 color = *(u16*)(thiz + 0xc);
        int m = color ? 0x10 : -0x10;
        int prod = *(Fix12i*)(thiz + 4) * m;
        int r = prod >> 12;
        if (r != 0) {
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4001050, 0x3f, r);
        } else {
            *(u16*)0x4000050 = 0;
            *(u16*)0x4001050 = 0;
        }
    }
}
