/* Deleting (D0) virtual destructor: set vptr, then Memory::operator_delete2(this),
 * return this.  vtable @ 0x020994cc (unnamed in reference; class unknown). */
extern void *_ZTV8dM3dGSph;
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct Obj { void *vtable; };

void *_ZN8dM3dGSphD0Ev(struct Obj *self)
{
    self->vtable = &_ZTV8dM3dGSph;
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
