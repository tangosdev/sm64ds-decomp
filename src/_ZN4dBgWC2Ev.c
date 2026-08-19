/* MeshColliderBase::MeshColliderBase() at 0x0203969c
 * Base constructor. Runs the base subobject constructor (func_02039624), then
 * installs the MeshColliderBase vtable. Returns this.
 */

extern int _ZTV16MeshColliderBase[];   // vtable
extern void func_02039624(void* self);  // base subobject constructor

void* _ZN16MeshColliderBaseC2Ev(void* self)
{
    *(int*)self = (int)_ZTV16MeshColliderBase; // set vptr
    func_02039624(self);
    return self;
}
