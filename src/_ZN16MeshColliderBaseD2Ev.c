/* MeshColliderBase::~MeshColliderBase() at 0x0203968c
 * Complete-object destructor (D1): it occupies slot 0 of this class's own vtable
 * (0x02099388, the ROM's dBgW), with D0 in slot 1. As the root of the hierarchy
 * it only resets the vptr; there is no base subobject to destruct -- which is why
 * it was mistakable for a D2. A D2 is never in a vtable at all.
 * See notes/dtor-variant-audit.md.
 */

extern int _ZTV16MeshColliderBase[]; // vtable

void _ZN16MeshColliderBaseD1Ev(void* self)
{
    *(int*)self = (int)_ZTV16MeshColliderBase; // set vptr
}
