//cpp
// @symbol _ZN4Door16CleanupResourcesEv
// recovered name: Door::CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "Door.h"
#include "SharedFilePtr.h"
/* recovered: renamed to Class_Method, vtable slot 3 */
/* Door::CleanupResources -- vtable slot 3, ov100 0x0214542c. This is the real
 * virtual override declared in include/Door.h rather than a free function
 * that only borrows the method's mangled symbol.
 *
 * FOLDED ONTO include/Door.h with the two files that used to include the
 * retired flat header. This one never included it -- it reached 0x008, 0x138
 * and 0x13c as raw `c + 0xNN` instead -- so it was invisible to a grep for
 * daDoor_c, and leaving it would have kept a second spelling of Door's layout
 * alive in the tree. Byte-exact under the pinned 2004/b56 before and after.
 *
 * `delete key` IS the ROM's `ldr r1,[r0]; ldr r1,[r1,#4]; blx r1` -- vtable
 * slot 1, Model's deleting destructor -- and it is also where the doubled
 * null check at 0x02149b04/0x02149b0c comes from: the explicit `if` emits one
 * and `delete` emits its own. The old source spelt that as a local `struct V`
 * with two dummy virtuals plus a hand-written second `if (obj != 0)`, which
 * produced the same bytes for the wrong reason. include/Door.h now types
 * 0x138 as Model*, so the shadow struct is gone. */
struct Elem { void* a; void* b; char pad[8]; };
extern Elem data_ov100_02148204[];
extern "C" {
extern void* data_ov100_02148744;
}
int Door::CleanupResources() {
  Elem* e = &data_ov100_02148204[param1];
  Model* key;
  ((SharedFilePtr *)(e->a))->Release();
  ((SharedFilePtr *)(&data_ov100_02148744))->Release();
  key = mKeyModel;
  if (key != 0) {
    delete key;
    ((SharedFilePtr *)(e->b))->Release();
  }
  if (mKeyFile != 0) {
    unsigned int v = param1;
    if (v >= 9 && v <= 0xd) UnloadKeyModels(v - 7);
    ((SharedFilePtr *)(mKeyFile))->Release();
  }
  return 1;
}
