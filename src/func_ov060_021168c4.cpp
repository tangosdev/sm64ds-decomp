//cpp
// @symbol func_ov060_021168c4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Actr.h"

struct dCc_c;
struct dActor_c;
struct dActor_c {
    static dActor_c* FindWithActorID(unsigned int id, dActor_c* a);
    static dActor_c* FindWithID(unsigned int id);
    void UpdatePos(dCc_c* c);
};
struct fBase_c { void MarkForDestruction(); };
extern "C" int func_ov060_02116518(char* c, int a, int b, int d);
extern "C" {
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
}

extern "C" void func_ov060_021168c4(char* c)
{
    char* r4;
    if (*(unsigned short*)(c + 0x374) == 0) {
        dActor_c* a = dActor_c::FindWithActorID(0x117, 0);
        r4 = (char*)a;
        *(int*)(c + 0x2cc) = *(int*)(r4 + 4);
    } else {
        r4 = (char*)dActor_c::FindWithID(*(unsigned int*)(c + 0x2cc));
    }
    if (((dBgCh_Actr*)(c + 0x110))->JustHitGround() != 0) {
        *(int*)(c + 0xa8) = 0x1e000;
    }
    ((dActor_c*)c)->UpdatePos((dCc_c*)0);
    func_ov060_02116518(c, 0xa6, 0, 0x32000);
    if (func_ov060_021172c8((unsigned char*)c, 0x96) != 0) {
        ((fBase_c*)c)->MarkForDestruction();
    }
    if (r4 == 0) return;
    if (*(int*)(r4 + 0x410) != 0) return;
    if (Vec3_HorzDist((Vector3*)(c + 0x5c), (Vector3*)(r4 + 0x5c)) >= 0x96000) return;
    ((fBase_c*)c)->MarkForDestruction();
}
