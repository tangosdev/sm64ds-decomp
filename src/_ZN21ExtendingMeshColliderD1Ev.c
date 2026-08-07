// @symbol _ZN21ExtendingMeshColliderD1Ev
/* recovered: named members + shared header */
/* ExtendingMeshCollider::~ExtendingMeshCollider() at 0x0203ab68
 * Complete-object destructor. Installs the ExtendingMeshCollider vtable, then
 * runs the base subobject destructor (_ZN18MovingMeshColliderD2Ev). Returns this.
 */

extern int _ZTV21ExtendingMeshCollider[]; // vtable
extern void _ZN18MovingMeshColliderD2Ev(void* self);     // base subobject destructor

void* _ZN21ExtendingMeshColliderD1Ev(void* self)
{
    *(int*)self = (int)_ZTV21ExtendingMeshCollider; // set vptr
    _ZN18MovingMeshColliderD2Ev(self);
    return self;
}
