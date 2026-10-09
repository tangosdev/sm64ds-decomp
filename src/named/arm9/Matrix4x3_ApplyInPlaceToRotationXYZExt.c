extern void Matrix4x3_FromRotationZ(void *m, int angle);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_FromRotationX(void *m, int angle);
extern void MulMat4x3Mat4x3(void *a, void *b, void *c);

struct Mtx { int v[12]; };

void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z)
{
    struct Mtx tmp;
    if (z != 0) {
        Matrix4x3_FromRotationZ(&tmp, z);
        MulMat4x3Mat4x3(&tmp, m, m);
    }
    if (y != 0) {
        Matrix4x3_FromRotationY(&tmp, y);
        MulMat4x3Mat4x3(&tmp, m, m);
    }
    if (x != 0) {
        Matrix4x3_FromRotationX(&tmp, x);
        MulMat4x3Mat4x3(&tmp, m, m);
    }
}
