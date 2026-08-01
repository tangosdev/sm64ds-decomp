extern void *data_02099d90;
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct Heap {
    void *vtable;
};

void *_ZN4HeapD0Ev(struct Heap *self)
{
    self->vtable = &data_02099d90;
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
