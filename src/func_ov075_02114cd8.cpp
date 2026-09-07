//cpp
// @symbol func_ov075_02114cd8
/* recovered: shared common types */
#include "common.h"

struct Obj;
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *mf, short angY);
struct BlendModelAnim { void Advance(); };
struct Animation { void Advance(); };
struct ShadowModel { void InitModel(Matrix4x3 *m, int b, int c, int d, unsigned int e); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(void *, Matrix4x3 *m, int b, int c, int d, unsigned int e);

extern "C" Matrix4x3 data_020a0e68;

struct C0 {};
typedef void (C0::*PMF)();
struct PmfRow { PMF pmf; };
extern "C" PmfRow data_ov075_0211d56c[];
extern "C" PmfRow data_ov075_0211d53c[];

extern "C" void func_ov075_02114cd8(void *self)
{
    char *s = (char*)self;
    *(unsigned char*)(s + 0x155) = 0;
    (((C0*)self)->*data_ov075_0211d56c[*(int*)(s + 0x110)].pmf)();
    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(s + 0x118) >> 3,
                              *(int*)(s + 0x11c) >> 3, *(int*)(s + 0x120) >> 3);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(s + 0x150));
    *(Matrix4x3*)(s + 0x1c) = data_020a0e68;
    (((C0*)self)->*data_ov075_0211d53c[*(int*)(s + 0x114)].pmf)();
    ((BlendModelAnim*)self)->Advance();
    ((Animation*)(s + 0xd4))->Advance();
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j((ShadowModel*)(s + 0xe8), (Matrix4x3*)(s + 0x1c), 0x50000, 0x1f4000, 0x50000, 0xf);
}
