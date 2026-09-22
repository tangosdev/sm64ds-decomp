//cpp
// @symbol func_ov102_0214ce60
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;


struct Player;
struct ShadowModel;
struct Matrix4x3;

struct dActor_c {
    Matrix4x3& UpdateCarry(Player& p, const Vector3& v);
    void DropShadowRadHeight(ShadowModel& sm, Matrix4x3& m, Fix12i a, int b, unsigned int c);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, ShadowModel& sm, Matrix4x3& m, Fix12i a, int b, unsigned int c);

extern "C" void Matrix4x3_FromRotationY(void* m, int angle);

extern "C" void func_ov102_0214ce60(char* c)
{
    int has = (*(int*)(c + 0xb0) & 0x4000) != 0;
    if (has) {
        Vector3 v;
        v.x = 0x1e000;
        v.y = -0x1e000;
        v.z = 0x32000;
        *(Matrix4x3*)(c + 0x31c) = ((dActor_c*)c)->UpdateCarry(*(Player*)*(void**)(c + 0x3c0), v);
    } else {
        Matrix4x3_FromRotationY(c + 0x31c, *(short*)(c + 0x8e));
        *(int*)(c + 0x340) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x344) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x348) = *(int*)(c + 0x64) >> 3;
    }
    int drop = (*(int*)(c + 0xb0) & 0x40000) != 0;
    if (drop) return;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j((dActor_c*)c, *(ShadowModel*)(c + 0x350), *(Matrix4x3*)(c + 0x31c), 0x50000, 0x50000, 0xf);
}
