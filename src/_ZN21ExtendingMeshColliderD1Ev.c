// @symbol _ZN21ExtendingMeshColliderD1Ev
/* recovered: named members + shared header */
#include "ExtendingMeshCollider.h"
/* ExtendingMeshCollider::~ExtendingMeshCollider() at 0x0203ab68
 * Complete-object destructor. Installs the ExtendingMeshCollider vtable, then
 * runs the base subobject destructor (func_0203a420). Returns this.
 */

extern int _ZTV21ExtendingMeshCollider[]; // vtable
extern void func_0203a420(void* self);     // base subobject destructor

void* _ZN21ExtendingMeshColliderD1Ev(void* self)
{
    *(int*)self = (int)_ZTV21ExtendingMeshCollider; // set vptr
    func_0203a420(self);
    return self;
}
