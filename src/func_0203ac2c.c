/* Deleting (D0) virtual destructor: set vptr, then Memory::operator_delete2(this),
 * return this.  vtable @ 0x020994cc (unnamed in reference; class unknown). */
extern void *data_020994cc;
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct Obj { void *vtable; };

void *func_0203ac2c(struct Obj *self)
{
    self->vtable = &data_020994cc;
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
