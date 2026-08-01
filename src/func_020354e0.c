/* Deleting (D0) virtual destructor: set vptr, then Memory::operator_delete2(this),
 * return this.  vtable @ 0x020991d8 (unnamed in reference; class unknown). */
extern void *data_020991d8;
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct Obj { void *vtable; };

void *func_020354e0(struct Obj *self)
{
    self->vtable = &data_020991d8;
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
