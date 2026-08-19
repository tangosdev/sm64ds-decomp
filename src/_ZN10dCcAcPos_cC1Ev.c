typedef struct MovingCylinderClsnWithPos {
    void *vtable;
    char pad[0x30 - 4];
    void *owner;
    int posX;
    int posY;
    int posZ;
} MovingCylinderClsnWithPos;

extern void *_ZTV25MovingCylinderClsnWithPos[];
extern MovingCylinderClsnWithPos* _ZN18MovingCylinderClsnC2Ev(MovingCylinderClsnWithPos* self);

MovingCylinderClsnWithPos* _ZN25MovingCylinderClsnWithPosC1Ev(MovingCylinderClsnWithPos* self) {
    _ZN18MovingCylinderClsnC2Ev(self);
    self->vtable = (void*)_ZTV25MovingCylinderClsnWithPos;
    return self;
}
