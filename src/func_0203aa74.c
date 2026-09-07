typedef int s32;
typedef s32 Fix12i;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { s32 m[12]; } Matrix4x3;

/* dBgW_KcMbgSclY fields:
 *   0x168 = clsnInvMat (Matrix4x3)
 *   0x1cc = invScaleY  (Fix12i)
 */
struct dBgW_KcMbgSclY {
    char pad[0x168];
    Matrix4x3 clsnInvMat;  /* 0x168, size 0x30 */
    char pad2[0x34];       /* 0x198..0x1cb */
    Fix12i invScaleY;      /* 0x1cc */
};

extern void MulVec3Mat4x3(const Vector3* v, const Matrix4x3* m, Vector3* res); /* 0x02052858 */

void func_0203aa74(struct dBgW_KcMbgSclY* thiz, Vector3* v, Vector3* res)
{
    MulVec3Mat4x3(v, &thiz->clsnInvMat, res);
    Fix12i isy = thiz->invScaleY;
    if (isy == 0x1000)
        return;
    Fix12i ry = res->y;
    res->y = (Fix12i)(((long long)ry * isy + 0x800) >> 12);
}
