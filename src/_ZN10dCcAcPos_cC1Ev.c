typedef struct dCcAcPos_c {
    void *vtable;
    char pad[0x30 - 4];
    void *owner;
    int posX;
    int posY;
    int posZ;
} dCcAcPos_c;

extern void *_ZTV10dCcAcPos_c[];
extern dCcAcPos_c* _ZN7dCcAc_cC2Ev(dCcAcPos_c* self);

dCcAcPos_c* _ZN10dCcAcPos_cC1Ev(dCcAcPos_c* self) {
    _ZN7dCcAc_cC2Ev(self);
    self->vtable = (void*)_ZTV10dCcAcPos_c;
    return self;
}
