// @symbol func_020084b0
/* recovered: shared common types */
#include "common.h"
// func_020084b0 @ 0x020084b0 - Camera/dActor_c member, size 0x50.
// Reads a packed unaligned Vector3_16 (x,y,z s16) from a data record,
// converts each component to Fix12i (<<12) and stores them as a Vector3
// at offset 0x8c of the target object. Returns 1.

typedef short s16;
typedef int s32;



struct Camera {
    char pad[0x8c];
    struct Vector3 vec;   // 0x8c
};

extern s16 ReadUnalignedShort(const void *from);   // 0x0200e768

s32 func_020084b0(struct Camera *self, const unsigned char *data)
{
    s32 y, z, x;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data)     << 12;
    self->vec.x = x;
    self->vec.y = y;
    self->vec.z = z;
    return 1;
}