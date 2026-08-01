#include "types.h"
typedef struct Mtx43 { int w[12]; } Mtx43;

void Matrix4x3_ApplyInPlaceToRotationX(void*, s16);
void Matrix4x3_ApplyInPlaceToRotationY(void*, s16);
void Matrix4x3_ApplyInPlaceToRotationZ(void*, s16);
void Matrix4x3_ApplyInPlaceToScale(void*, int, int, int);
void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void*, void*, s16);

void func_ov079_02124008(char* c)
{
    s16 a = (s16)(*(s16*)(c + 0x8c) + *(s16*)(c + 0x3e0));
    int s;
    u8 n = *(u8*)(c + 0x414);
    int off;

    if (a < 0) a *= -1;
    s = (s16)(0x4000 - a);

    if (n) {
        if (*(int*)(c + 0x3b0) == 0) s = s / 32 + 0x1000;
        else s = s / 48 + 0x1000;
    } else {
        s = s / 102;
        s = s + 0x1000;
    }

    off = (n + 1) * ((s - 0x1000) * 0x168);
    *(Mtx43*)(c + 0x5e0) = *(Mtx43*)(c + 0x2e8);

    Matrix4x3_ApplyInPlaceToRotationY(c + 0x5e0, *(s16*)(c + 0x3e2));
    if (*(int*)(c + 0x3b0) != 1 && *(int*)(c + 0x3b0) != 7) {
        Matrix4x3_ApplyInPlaceToRotationX(c + 0x5e0, *(s16*)(c + 0x3e0));
        Matrix4x3_ApplyInPlaceToRotationZ(c + 0x5e0, (s16)(*(s16*)(c + 0x3e4) >> 1));
    }
    Matrix4x3_ApplyInPlaceToScale(c + 0x5e0, 0x1000, s, 0x1000);

    *(int*)(c + 0x604) = *(int*)(c + 0x5c);
    *(int*)(c + 0x608) = *(int*)(c + 0x60) + *(int*)(c + 0x410) - off;
    *(int*)(c + 0x60c) = *(int*)(c + 0x64);

    _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(c + 0x418, c + 0x5e0, *(s16*)(c + 0x3e2));
}
