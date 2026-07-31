#include "types.h"
typedef struct {
    s32 m[12]; /* 4x3 matrix, 48 bytes */
} Matrix4x3;

extern Matrix4x3 VIEW_MATRIX_ASR_3;
extern Matrix4x3 INV_VIEW_MATRIX_ASR_3;
extern void InvMat4x3(Matrix4x3* dst, Matrix4x3* inv);

void _Z13CopyToViewMatPK9Matrix4x3(const Matrix4x3* src) {
    VIEW_MATRIX_ASR_3 = *src;
    InvMat4x3(&VIEW_MATRIX_ASR_3, &INV_VIEW_MATRIX_ASR_3);
}
