//cpp
extern "C" {
extern void *_ZTV6Dorrie;
extern void func_ov065_021180b8(void);
void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
void _ZN18MovingCylinderClsnD1Ev(void*);
void _ZN12WithMeshClsnD1Ev(void*);
void func_0207328c(void*, int, int, void*);
void _ZN9ModelAnimD1Ev(void*);
void _ZN5ActorD1Ev(void*);
void *_ZN6DorrieD1Ev(void* thiz);
}
void *_ZN6DorrieD1Ev(void* thiz) {
    char *c = (char*)thiz;
    *(void**)c = &_ZTV6Dorrie;
    _ZN25MovingCylinderClsnWithPosD1Ev(c + 0x1140);
    _ZN18MovingCylinderClsnD1Ev(c + 0x110c);
    _ZN12WithMeshClsnD1Ev(c + 0xf50);
    func_0207328c(c + 0x150, 7, 0x200, (void*)func_ov065_021180b8);
    _ZN9ModelAnimD1Ev(c + 0xec);
    _ZN5ActorD1Ev(c);
    return c;
}
