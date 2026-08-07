// @symbol _ZN18MovingMeshColliderD1Ev
/* recovered: named members + shared header */
/* MovingMeshCollider::~MovingMeshCollider() at 0x0203a470
 * Complete-object destructor. Installs the MovingMeshCollider vtable, then runs
 * the base subobject destructor (_ZN12MeshColliderD2Ev). Returns this.
 */

extern int _ZTV18MovingMeshCollider[];  // vtable
extern void _ZN12MeshColliderD2Ev(void* self);   // base subobject destructor

void* _ZN18MovingMeshColliderD1Ev(void* self)
{
    *(int*)self = (int)_ZTV18MovingMeshCollider; // set vptr
    _ZN12MeshColliderD2Ev(self);
    return self;
}
