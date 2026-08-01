//cpp
// @symbol func_ov081_02127070
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;
typedef short s16;

extern "C" Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern "C" s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern "C" void _Z14ApproachLinearRsss(s16& v, s16 target, s16 step);
struct CylinderClsn { void Clear(); void Update(); };
struct Animation { void Advance(); };
struct Actor { void UpdatePos(CylinderClsn* c); };

struct Obj {
    char pad0[0x5c];
    Vector3 pos;          // 0x5c
    char pad1[0x26];      // 0x68 -> 0x8e
    s16 f8e;              // 0x8e
    char pad2[4];         // 0x90 -> 0x94
    s16 f94;              // 0x94
    char pad3[2];         // 0x96 -> 0x98
    Fix12i f98;           // 0x98
    char pad4[0x124-0x9c];
    char anim[0x1b0-0x124];   // 0x124
    char clsn[0x1e4-0x1b0];   // 0x1b0
    char field1e4[0x3d0-0x1e4];
    Vector3 target;       // 0x3d0
};

extern "C" int func_ov081_02127070(Obj* self)
{
    Fix12i d = Vec3_Dist(&self->pos, &self->target);
    if (d < self->f98) {
        self->f8e = Vec3_HorzAngle(&self->pos, &self->target);
        self->f94 = self->f8e;
        self->f98 = d;
    } else {
        _Z14ApproachLinearRsss(self->f8e, Vec3_HorzAngle(&self->pos, &self->target), 0x2bc);
        self->f94 = self->f8e;
    }
    ((Animation*)self->anim)->Advance();
    ((Actor*)self)->UpdatePos((CylinderClsn*)self->clsn);
    func_ov081_02126950(self, self->field1e4);
    func_ov081_02126758(self);
    if (d < 0xa000) {
        func_ov081_021265c8(self);
    }
    ((CylinderClsn*)self->clsn)->Clear();
    ((CylinderClsn*)self->clsn)->Update();
    return 1;
}
