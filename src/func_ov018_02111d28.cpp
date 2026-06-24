//cpp
struct Matrix4x3 { int m[12]; };
struct ShadowModel;
struct Actor {
    void DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &mf, int c, int d, unsigned int e);
};
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *mf, short angY);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *mf, short angX);
extern "C" Matrix4x3 data_020a0e68;

extern "C" void func_ov018_02111d28(Actor *self)
{
    char *s = (char*)self;
    Matrix4x3_FromRotationY(s + 0xf0, *(short*)(s + 0x8e));
    *(int*)(s + 0x114) = *(int*)(s + 0x5c) >> 3;
    *(int*)(s + 0x118) = *(int*)(s + 0x60) >> 3;
    *(int*)(s + 0x11c) = *(int*)(s + 0x64) >> 3;
    self->DropShadowRadHeight(*(ShadowModel*)(s + 0x14c), *(Matrix4x3*)(s + 0xf0),
                              0x140000, 0x50000, 0xf);
    if (*(short*)(s + 0x380) != 0 || *(short*)(s + 0x382) != 0) {
        Matrix4x3 *dst = (Matrix4x3*)((*(char**)(s + 0xe8)) + 0xf0);
        data_020a0e68 = *dst;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(s + 0x382));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(s + 0x380));
        *(Matrix4x3*)((*(char**)(s + 0xe8)) + 0xf0) = data_020a0e68;
    }
}
