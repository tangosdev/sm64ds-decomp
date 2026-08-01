// @symbol func_ov072_0211f330
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;
typedef long long s64;

extern void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern char* _ZNK12WithMeshClsn14GetFloorResultEv(void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, struct Vector3* v);
extern int _ZN4cstd4fdivEii(int a, int b);

void func_ov072_0211f330(char* c, void* mc) {
  struct Vector3 n;
  char* fr;
  WithMeshClsn_UpdateContinuous_Veneer(mc);
  if (_ZNK12WithMeshClsn10IsOnGroundEv(mc) == 0) return;
  fr = _ZNK12WithMeshClsn14GetFloorResultEv(mc);
  _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr+4, &n);
  if (n.y == 0) return;
  {
    int a = (int)(((s64)n.x * *(int*)(c+0xa4) + 0x800) >> 12);
    int b = (int)(((s64)n.z * *(int*)(c+0xac) + 0x800) >> 12);
    *(int*)(c+0xa8) = -(_ZN4cstd4fdivEii(a + b, n.y) + 0x8000);
  }
}
