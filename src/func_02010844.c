typedef signed int s32;
typedef signed short s16;
typedef unsigned short u16;
typedef signed long long s64;

typedef struct {
    s32 x;   /* 0x00 */
    s32 y;   /* 0x04 */
    s32 z;   /* 0x08 */
} Vector3;

extern s16 data_02082214[];

extern s32 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 Vec3_HorzLen(Vector3* v);

s32 func_02010844(void* unused, Vector3* v, s16 angle)
{
    s32 dir;
    s32 horzLen;

    dir = _ZN4cstd5atan2E5Fix12IiES1_(v->x, v->z);
    angle = (s16)(dir - angle);
    horzLen = Vec3_HorzLen(v);
    return _ZN4cstd5atan2E5Fix12IiES1_(
        (s32)(((s64)horzLen * data_02082214[((u16)angle >> 4) * 2 + 1] + 0x800) >> 12),
        v->y);
}
