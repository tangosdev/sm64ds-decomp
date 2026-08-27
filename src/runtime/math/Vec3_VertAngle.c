typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef signed short s16;

typedef struct { s32 x, y, z; } Vector3;

extern void Vec3_Sub(Vector3 *res, const Vector3 *v0, const Vector3 *v1);
extern s32 Vec3_HorzLen(const Vector3 *v0);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);

s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0)
{
    Vector3 temp;
    Vec3_Sub(&temp, v1, v0);
    s32 horzLen = Vec3_HorzLen(&temp);
    return _ZN4cstd5atan2E5Fix12IiES1_(temp.y, horzLen);
}