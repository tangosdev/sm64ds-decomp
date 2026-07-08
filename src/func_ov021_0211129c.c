typedef int Fix12;
typedef short s16;
struct Quaternion;
struct Matrix4x3;
extern void Matrix4x3_FromQuaternion(const struct Quaternion* q, struct Matrix4x3* mF);
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *mF, s16 angX);
extern void Matrix4x3_ApplyInPlaceToRotationZ(struct Matrix4x3 *mF, s16 angZ);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, s16 angY);
extern void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void* self, void* mtx, s16 a);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, Fix12 x, Fix12 y, Fix12 z);

struct M48 { int w[12]; };
extern struct M48 MATRIX_SCRATCH_PAPER;
extern const int data_ov021_02114a20[];
extern const s16 data_ov021_02114740[];

void func_ov021_0211129c(char* c) {
    struct M48 mtx;
    int i;
    char* tr;
    char* obj;
    char* col;
    char* transform;
    volatile int v[3];

    Matrix4x3_FromQuaternion((struct Quaternion*)(c+0xc4c), (struct Matrix4x3*)&mtx);
    Matrix4x3_FromTranslation((struct Matrix4x3*)&MATRIX_SCRATCH_PAPER, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
    MulMat4x3Mat4x3(&mtx, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    Matrix4x3_ApplyInPlaceToRotationX((struct Matrix4x3*)&MATRIX_SCRATCH_PAPER, *(s16*)(c+0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ((struct Matrix4x3*)&MATRIX_SCRATCH_PAPER, *(s16*)(c+0x90));
    Matrix4x3_ApplyInPlaceToRotationY((struct Matrix4x3*)&MATRIX_SCRATCH_PAPER, *(s16*)(c+0x8e));
    *(struct M48*)(c+0x2ec) = MATRIX_SCRATCH_PAPER;
    _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(c+0x124, c+0x2ec, *(s16*)(c+0x8e));

    tr = (char*)&data_ov021_02114a20[0];
    i = 0;
    obj = c;
    col = c + 0x460;
    transform = c + 0x520;
    for (; i < 4; i++) {
        int ny;
        v[0] = *(int*)(tr);
        ny = *(int*)(tr+4);
        v[1] = ny;
        v[2] = *(int*)(tr+8);
        if (*(signed char*)(c+0xc00+0x7a) == i) {
            if (*(unsigned char*)(c+0xc7d) == 0) {
                v[1] = ny - 0x1e000;
            }
        }
        *(struct M48*)&MATRIX_SCRATCH_PAPER = *(struct M48*)(c+0x2ec);
        Matrix4x3_ApplyInPlaceToTranslation((struct Matrix4x3*)&MATRIX_SCRATCH_PAPER, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToRotationY((struct Matrix4x3*)&MATRIX_SCRATCH_PAPER, data_ov021_02114740[i]);
        *(struct M48*)(obj+0x460) = MATRIX_SCRATCH_PAPER;
        _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(transform, col, *(s16*)(c+0x8e));
        tr += 0xc;
        obj += 0x30;
        col += 0x30;
        transform += 0x1c8;
    }
}
