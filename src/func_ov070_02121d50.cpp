//cpp
// @symbol func_ov070_02121d50
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

struct SurfaceInfo { void CopyNormalTo(Vector3& v) const; };
struct WithMeshClsn {
    bool IsOnGround() const;
    SurfaceInfo* GetFloorResult() const;
    bool IsOnWall() const;
};
namespace cstd { int fdiv(int a, int b); }

extern "C" void WithMeshClsn_UpdateContinuous_Veneer(void* c);

extern "C" void func_ov070_02121d50(int* self, WithMeshClsn* clsn) {
    Vector3 n;
    WithMeshClsn_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        ((SurfaceInfo*)((char*)clsn->GetFloorResult() + 4))->CopyNormalTo(n);
        if (n.y != 0) {
            int a = (int)(((long long)n.x * self[0x29] + 0x800) >> 12);
            int b = (int)(((long long)n.z * self[0x2b] + 0x800) >> 12);
            self[0x2a] = -(cstd::fdiv(a + b, n.y) + 0x8000);
        }
    }
    if (clsn->IsOnWall())
        func_ov070_02121c8c(self);
}
