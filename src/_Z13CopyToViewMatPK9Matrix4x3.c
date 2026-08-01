#include "types.h"
typedef struct {
    s32 m[12]; /* 4x3 matrix, 48 bytes */
} Matrix4x3;

extern Matrix4x3 data_0209b3ec;
extern Matrix4x3 data_0209b41c;
extern void InvMat4x3(Matrix4x3* dst, Matrix4x3* inv);

void _Z13CopyToViewMatPK9Matrix4x3(const Matrix4x3* src) {
    data_0209b3ec = *src;
    InvMat4x3(&data_0209b3ec, &data_0209b41c);
}
