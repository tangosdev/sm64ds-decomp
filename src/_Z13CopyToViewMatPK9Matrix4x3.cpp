//cpp
// @symbol _Z13CopyToViewMatPK9Matrix4x3
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free free functions).
//
// Free function: CopyToViewMat(const Matrix4x3*). Copies *src into the live
// view matrix at data_0209b3ec, then writes the inverse into data_0209b41c.
#include "types.h"

typedef struct {
    s32 m[12]; /* 4x3 matrix, 48 bytes */
} Matrix4x3;

extern "C" {
extern Matrix4x3 data_0209b3ec;
extern Matrix4x3 data_0209b41c;
extern void InvMat4x3(Matrix4x3* dst, Matrix4x3* inv);
}

void CopyToViewMat(const Matrix4x3* src)
{
    data_0209b3ec = *src;
    InvMat4x3(&data_0209b3ec, &data_0209b41c);
}
