typedef struct { int x, y, z; } Vector3;
struct Matrix4x3 { int m[12]; };

extern void Matrix4x3_FromRotationY(struct Matrix4x3 *m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *m, short angX);
extern void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);

extern struct Matrix4x3 data_020a0e68;
extern void* data_ov062_0211e104[];

int func_ov062_0211c594(char *c) {
    Vector3 in;
    Vector3 out;

    *(int*)(c + 0xa4) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = 0;

    if (*(unsigned char*)(c + 0x446) == 1) {
        in.x = 0; in.y = 0; in.z = 0;
        out.x = 0; out.y = 0; out.z = 0;
        in.z = 0x14000;
        Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, -0x4000);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        *(int*)(c + 0xa8) = out.y;
        *(unsigned short*)(c + 0x100) = 0x3c;
    }

    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x334, data_ov062_0211e104[1], 4, 0, 0x1000, 0);
    return 1;
}
