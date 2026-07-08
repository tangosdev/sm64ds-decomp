typedef unsigned char u8;
struct Vector3 { int x, y, z; };
struct Matrix4x3 { int w[12]; };
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
extern void InvMat4x3(struct Matrix4x3 *d, struct Matrix4x3 *s);
extern void MulVec3Mat4x3(struct Vector3 *v, struct Matrix4x3 *m, struct Vector3 *out);
extern void Quaternion_FromVector3(int *q, struct Vector3 *a, struct Vector3 *b);
extern void Quaternion_Normalize(int *q);

void func_ov002_020b62cc(unsigned char *self, unsigned char *arg)
{
    struct Vector3 v;
    struct Vector3 axis;
    *(u8 *)(self + 0x34c) = 1;
    MATRIX_SCRATCH_PAPER = *(struct Matrix4x3 *)(self + 0x2ec);
    InvMat4x3(&MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    MulVec3Mat4x3((struct Vector3 *)(arg + 0x5c), &MATRIX_SCRATCH_PAPER, &v);
    v.y = v.y << 1;
    axis.x = 0;
    axis.z = 0;
    axis.y = 0x1000;
    Quaternion_FromVector3((int *)(self + 0x330), &axis, &v);
    Quaternion_Normalize((int *)(self + 0x330));
    *(u8 *)(self + 0x34d) = 0xa;
}
