extern int func_0207328c(void* c, int a, int b, void* d);
extern void* _ZTV8Goomboss[];
extern void func_020072c0(void);
extern int _ZN12WithMeshClsnD1Ev(void* c);
extern int _ZN18TextureTransformerD1Ev(void* c);
extern int _ZN15TextureSequenceD1Ev(void* c);
extern int _ZN15MaterialChangerD1Ev(void* c);
extern int _ZN11ShadowModelD1Ev(void* c);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void* c);
extern int _ZN9ModelAnimD1Ev(void* c);
extern int func_ov002_020aed18(int* x);
extern int data_020a0eac;
extern int _ZN6Memory10DeallocateEPvP4Heap(void* p, void* h);
int _ZN8GoombossD0Ev(void* c)
{
    *(void**)c = _ZTV8Goomboss;
    _ZN12WithMeshClsnD1Ev((char*)c+0x40c);
    _ZN18TextureTransformerD1Ev((char*)c+0x3f8);
    _ZN15TextureSequenceD1Ev((char*)c+0x3e4);
    _ZN15MaterialChangerD1Ev((char*)c+0x3d0);
    func_0207328c((char*)c+0x3ac, 3, 0xc, (void*)func_020072c0);
    func_0207328c((char*)c+0x274, 3, 0x28, (void*)_ZN11ShadowModelD1Ev);
    _ZN9ModelAnimD1Ev((char*)c+0x210);
    func_0207328c((char*)c+0x110, 4, 0x40, (void*)_ZN25MovingCylinderClsnWithPosD1Ev);
    func_ov002_020aed18((int*)c);
    _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac);
    return (int)c;
}
