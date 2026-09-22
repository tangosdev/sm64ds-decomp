typedef int s32;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { Vector3 c0, c1, c2, c3; } Matrix4x3;

/* dBgW_KcMbg: newTransform at 0x54 */
struct dBgW_KcMbg {
    char pad[0x54];
    Matrix4x3 newTransform;  /* 0x54 */
};

extern void MulVec3Mat4x3(const Vector3* v, const Matrix4x3* m, Vector3* res); /* 0x02052858 */

void func_02039db8(struct dBgW_KcMbg* thiz, Vector3* vec, Vector3* res)
{
    Matrix4x3 mat;
    mat = thiz->newTransform;
    mat.c3.x = 0;
    mat.c3.y = 0;
    mat.c3.z = 0;
    MulVec3Mat4x3(vec, &mat, res);
}
