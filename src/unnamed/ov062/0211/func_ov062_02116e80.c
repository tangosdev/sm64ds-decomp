typedef int Fix12i;
typedef short s16;
struct Matrix4x3 { char data[0x30]; };

extern void Matrix4x3_FromRotationY(struct Matrix4x3 *mF, s16 angY);
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);

void func_ov062_02116e80(void *c) {
    Matrix4x3_FromRotationY((struct Matrix4x3*)((char*)c + 0x31c), *(s16*)((char*)c + 0x8e));
    *(Fix12i*)((char*)c + 0x340) = *(Fix12i*)((char*)c + 0x5c) >> 3;
    *(Fix12i*)((char*)c + 0x344) = *(Fix12i*)((char*)c + 0x60) >> 3;
    *(Fix12i*)((char*)c + 0x348) = *(Fix12i*)((char*)c + 0x64) >> 3;
    MulMat4x3Mat4x3((struct Matrix4x3*)(*(int*)((char*)c + 0x314) + 0x90),
                    (struct Matrix4x3*)((char*)c + 0x31c),
                    (struct Matrix4x3*)((char*)c + 0x3fc));
    *(int*)((char*)c + 0xc8) = (int)((char*)c + 0x3fc);
}
