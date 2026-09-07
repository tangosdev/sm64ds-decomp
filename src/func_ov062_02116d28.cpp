//cpp
// @symbol func_ov062_02116d28
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;

struct ShadowModel;
struct dActor_c {
    void DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &mtx, Fix12 a, int b, unsigned int c);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, ShadowModel &sm, Matrix4x3 &mtx, Fix12 a, int b, unsigned int c);


extern Matrix4x3 IDENTITY_MATRIX4X3;

extern "C" void func_ov062_02116d28(char *c)
{
    *(Matrix4x3 *)(c + 0x390) = IDENTITY_MATRIX4X3;
    char *o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3b4) = *(int *)(o + 0x5c) >> 3;
    o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3b8) = *(int *)(o + 0x60) >> 3;
    o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3bc) = *(int *)(o + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j((dActor_c *)c, *(ShadowModel *)(c + 0x368), *(Matrix4x3 *)(c + 0x390), 0x12c000, 0x32000, 0xf);
}
