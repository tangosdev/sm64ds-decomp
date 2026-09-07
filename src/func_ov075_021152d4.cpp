//cpp
// @symbol func_ov075_021152d4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "Clipper.h"

struct Vector3;
namespace G3i {
    void PerspectiveW_(int a1, int a2, int a3, int a4, int a5, int a6, bool b, Matrix4x3 *m);
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int a1, int a2, int a3, int a4, int a5, int a6, bool b, Matrix4x3 *m);

    void LookAt_(const Vector3 *eye, const Vector3 *at, const Vector3 *up, bool b, Matrix4x3 *m);
}
extern "C" void _Z13CopyToViewMatPK9Matrix4x3(const Matrix4x3 *m);
extern "C" short data_02082614[];
extern "C" Clipper data_0209f43c;

extern "C" void func_ov075_021152d4(char *self)
{
    Matrix4x3 view;
    G3i::_ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(data_02082614[0xa], data_02082614[0xb], 0x1555, 0x1000, 0x1388000, 0x1000, true, (Matrix4x3*)0);
    G3i::LookAt_((Vector3*)(self + 0xf28), (Vector3*)&data_ov075_0211c660,
                 (Vector3*)(self + 0xf34), true, &view);
    _Z13CopyToViewMatPK9Matrix4x3(&view);
    data_0209f43c.Func_020156DC(0x1555, 0x105b, 0x1000, 0x1388000);
}
