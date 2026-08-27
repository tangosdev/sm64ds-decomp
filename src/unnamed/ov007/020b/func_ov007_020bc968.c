#include "types.h"
extern void Matrix4x3_LoadIdentity(void* mat);
extern void func_ov007_020c39f8(void* mat, int a, int b, int c);
extern void MulMat4x3Mat4x3(void* d, void* a, void* b);

void func_ov007_020bc968(char* c, void* other) {
    Matrix4x3_LoadIdentity(c + 0x54);
    func_ov007_020c39f8(c + 0x54, *(u16*)(c + 0x4c), *(u16*)(c + 0x4e), *(u16*)(c + 0x50));
    *(int*)(c + 0x78) = *(int*)(c + 0x34);
    *(int*)(c + 0x7c) = *(int*)(c + 0x38);
    *(int*)(c + 0x80) = *(int*)(c + 0x3c);
    MulMat4x3Mat4x3(c + 0x54, other, c + 0x54);
}
