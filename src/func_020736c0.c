/* Deleting (D0) virtual destructor: set vptr, then Memory::operator_delete2(this),
 * return this.  vtable @ 0x0209a744 (unnamed in reference; class unknown). */
extern void *_ZTVSt9type_info;
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct Obj { void *vtable; };

void *func_020736c0(struct Obj *self)
{
    self->vtable = &_ZTVSt9type_info;
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
