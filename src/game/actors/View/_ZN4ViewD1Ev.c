extern void *_ZTV4View;
extern void *data_0208e4b8;
extern void *_ZN7fBase_cD2Ev(void *self);

struct View {
    void *vtable;
};

void *_ZN4ViewD1Ev(struct View *self)
{
    self->vtable = &_ZTV4View;
    self->vtable = &data_0208e4b8;
    _ZN7fBase_cD2Ev(self);
    return self;
}
