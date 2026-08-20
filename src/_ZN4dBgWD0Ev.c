extern void *_ZTV4dBgW;
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct dBgW {
    void *vtable;
};

void *_ZN4dBgWD0Ev(struct dBgW *self)
{
    self->vtable = &_ZTV4dBgW;
    _ZN6Memory16operator_delete2EPv(self);
    return self;
}
