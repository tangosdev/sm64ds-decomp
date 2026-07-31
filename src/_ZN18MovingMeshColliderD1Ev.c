// @symbol _ZN18MovingMeshColliderD1Ev
/* recovered: named members + shared header */
#include "MovingMeshCollider.h"
/* MovingMeshCollider::~MovingMeshCollider() at 0x0203a470
 * Complete-object destructor. Installs the MovingMeshCollider vtable, then runs
 * the base subobject destructor (func_020397fc). Returns this.
 */

extern int _ZTV18MovingMeshCollider[];  // vtable
extern void func_020397fc(void* self);   // base subobject destructor

void* _ZN18MovingMeshColliderD1Ev(void* self)
{
    *(int*)self = (int)_ZTV18MovingMeshCollider; // set vptr
    func_020397fc(self);
    return self;
}
