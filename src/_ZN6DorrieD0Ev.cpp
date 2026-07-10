//cpp
extern "C" {
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern int func_0207328c(void *, int, int, void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void func_ov065_021180b8();
extern void *_ZTV6Dorrie[];
extern void *data_020a0eac;
void *_ZN6DorrieD0Ev(char *c);
void *_ZN6DorrieD0Ev(char *c) {
    *(void ***)c = _ZTV6Dorrie;
    _ZN25MovingCylinderClsnWithPosD1Ev(c + 0x1140);
    _ZN18MovingCylinderClsnD1Ev(c + 0x110c);
    _ZN12WithMeshClsnD1Ev(c + 0xf50);
    func_0207328c(c + 0x150, 7, 0x200, (void*)&func_ov065_021180b8);
    _ZN9ModelAnimD1Ev(c + 0xec);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
