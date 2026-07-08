typedef int s32;
typedef unsigned short u16;
typedef signed short s16;
typedef struct { s32 x, y, z; } Vector3;

extern void Vec3_Sub(Vector3 *res, const Vector3 *v0, const Vector3 *v1);
extern s32 Vec3_HorzLen(const Vector3 *v0);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);

extern s16 SINE_TABLE[];

#define A92 (*(u16*)((char*)c+0x92))
#define A94 (*(u16*)((char*)c+0x94))
#define V98 (*(s32*)((char*)c+0x98))

void func_ov091_02131cb0(void *c, const Vector3 *v0, const Vector3 *v1)
{
    Vector3 temp;
    Vec3_Sub(&temp, v0, v1);
    *(s16*)((char*)c+0x94) = _ZN4cstd5atan2E5Fix12IiES1_(temp.x, temp.z);
    *(s16*)((char*)c+0x92) = _ZN4cstd5atan2E5Fix12IiES1_(temp.y, Vec3_HorzLen(&temp));
    s32 r = (s32)(((long long)V98 * SINE_TABLE[(A92>>4)*2+1] + 0x800) >> 12);
    *(s32*)((char*)c+0xa4) = (s32)(((long long)r * SINE_TABLE[(A94>>4)*2] + 0x800) >> 12);
    *(s32*)((char*)c+0xa8) = (s32)(((long long)V98 * SINE_TABLE[(A92>>4)*2] + 0x800) >> 12);
    *(s32*)((char*)c+0xac) = (s32)(((long long)r * SINE_TABLE[(A94>>4)*2+1] + 0x800) >> 12);
}
