typedef int s32;
typedef s32 Fix12i;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { s32 m[12]; } Matrix4x3;

/* dBgW_KcMbgSclY fields (relative to object start):
 *   0x134 = ledgeMat (Matrix4x3)
 *   0x1c8 = scaleY   (Fix12i)
 */
struct dBgW_KcMbgSclY {
    char pad[0x134];
    Matrix4x3 ledgeMat; /* 0x134, size 0x30 */
    char pad2[0x64];    /* 0x164..0x1c7 */
    Fix12i scaleY;      /* 0x1c8 */
};

extern void MulVec3Mat4x3(const Vector3* v, const Matrix4x3* m, Vector3* res); /* 0x02052858 */

void func_0203aa10(struct dBgW_KcMbgSclY* thiz, const Vector3* v, Vector3* res)
{
    Vector3 tmp;
    tmp.x = v->x;
    tmp.y = v->y;
    tmp.z = v->z;
    if (thiz->scaleY != 0x1000)
    {
        Fix12i sy = thiz->scaleY;
        tmp.y = (Fix12i)(((long long)tmp.y * sy + 0x800) >> 12);
    }
    MulVec3Mat4x3(&tmp, &thiz->ledgeMat, res);
}
