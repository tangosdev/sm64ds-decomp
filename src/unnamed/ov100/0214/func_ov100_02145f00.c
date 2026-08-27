// @symbol func_ov100_02145f00
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern void Vec3_RotateYAndTranslate(struct Vector3* out, void* m, int ang, struct Vector3* in);

extern unsigned char data_0209f250;
extern char* data_0209f394[];
extern char data_020a0ebc[];

char* func_ov100_02145f00(char* c)
{
    struct Vector3 tmp;
    char* p = data_0209f394[data_0209f250];
    Vec3_Sub(&tmp, (struct Vector3*)(p + 0x5c), (struct Vector3*)(c + 0x5c));
    Vec3_RotateYAndTranslate((struct Vector3*)(c + 0x80), data_020a0ebc, (short)(-*(short*)(c + 0x8e)), &tmp);
    return p;
}
