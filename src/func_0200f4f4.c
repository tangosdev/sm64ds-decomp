#include "types.h"
/* 4x3 matrix: 12 s32 values = 48 bytes */
typedef struct {
    s32 m[12];
} Matrix4x3;

extern void Initialise3dGraphics(s32 mode);
extern void _ZN2GX13SetBankForTexEt(u16 val);
extern void _ZN2GX17SetBankForTexPlttEt(u16 val);
extern void func_02045d9c(void);
extern void InvMat4x3(Matrix4x3* dst, Matrix4x3* out);

extern Matrix4x3 gMatrix_02082128;
extern Matrix4x3 gMatrix_0209b3ec;
extern Matrix4x3 gMatrix_0209b41c;

void func_0200f4f4(void) {
    Initialise3dGraphics(0);
    _ZN2GX13SetBankForTexEt(3);
    _ZN2GX17SetBankForTexPlttEt(0x30);
    func_02045d9c();
    gMatrix_0209b3ec = gMatrix_02082128;
    InvMat4x3(&gMatrix_0209b3ec, &gMatrix_0209b41c);
}
