//cpp
struct Matrix4x3 { int m[12]; };
extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *mtx, short angle);

struct Obj {
    char pad0[0x5c];
    int posx;     // 0x5c
    int posy;     // 0x60
    int posz;     // 0x64
    char pad1[0x8e - 0x68];
    short angle;  // 0x8e
    char pad2[0xf0 - 0x90];
    Matrix4x3 dst; // 0xf0
    char pad3[0x140 - 0xf0 - sizeof(Matrix4x3)];
    Matrix4x3 mtx; // 0x140
};

extern "C" void func_ov002_020b9a1c(Obj *self) {
    Matrix4x3_FromRotationY(&self->mtx, self->angle);
    self->mtx.m[9] = self->posx >> 3;
    self->mtx.m[10] = self->posy >> 3;
    self->mtx.m[11] = self->posz >> 3;
    self->dst = self->mtx;
}
