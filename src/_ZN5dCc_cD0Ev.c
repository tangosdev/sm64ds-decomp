extern void *data_0208e6ec;
extern void *func_02014fa4(void *self);
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct CylinderClsn {
    void *vtable;
};

void *_ZN12CylinderClsnD0Ev(struct CylinderClsn *self)
{
    self->vtable = &data_0208e6ec;
    func_02014fa4(self);
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
