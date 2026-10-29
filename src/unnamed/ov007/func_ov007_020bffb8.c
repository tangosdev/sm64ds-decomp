#include "types.h"
extern s16 data_02082214[];

/* The 0xd8-byte camera func_ov007_020c023c allocates; names as in
 * func_ov007_020c0128, only the slots this file touches. */
typedef struct Ov007Camera {
    char pad_00[8];
    Vector3 eye;              /* 0x08 */
    Vector3 target;           /* 0x14 */
    Vector3 up;               /* 0x20 */
    int distance;             /* 0x2c */
    unsigned short elevation; /* 0x30 */
    unsigned short azimuth;   /* 0x32 */
} Ov007Camera;

typedef char Ov007Camera_size_must_be_0x34[sizeof(Ov007Camera) == 0x34 ? 1 : -1];

/* Places the eye on the orbit around the target: distance away, at the
 * camera's elevation and azimuth. c keeps the char * spelling of the
 * definition; `cam` is the typed view. out/base stay pointers into eye and
 * target: spelled cam->eye.x .. cam->target.z directly, the function
 * compiles 16 bytes short. */
void func_ov007_020bffb8(char *c)
{
    Ov007Camera *cam = (Ov007Camera *)c;
    s32 *out = &cam->eye.x;
    s32 *base = &cam->target.x;
    s32 angleA = cam->elevation;
    s32 angleB = cam->azimuth;
    s32 indexA;
    s32 indexB;
    s32 trigAPlus;

    angleA >>= 4;
    angleB >>= 4;
    indexA = angleA << 1;
    indexB = angleB << 1;
    trigAPlus = data_02082214[indexA + 1];

    {
        s32 initialX = (trigAPlus * cam->distance) >> 12;
        s32 trigB = data_02082214[indexB];
        s32 storedX;
        s32 product;

        out[0] = initialX;
        storedX = out[0];
        product = storedX * trigB;
        out[0] = base[0] + (product >> 12);
    }
    {
        s32 trigA = data_02082214[indexA];
        s32 product = trigA * cam->distance;
        out[1] = base[1] + (product >> 12);
    }
    {
        s32 initialZ = (trigAPlus * cam->distance) >> 12;
        s32 trigB = data_02082214[indexB + 1];
        s32 storedZ;
        s32 product;

        out[2] = initialZ;
        storedZ = out[2];
        product = storedZ * trigB;
        out[2] = base[2] + (product >> 12);
    }
}
