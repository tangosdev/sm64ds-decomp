extern int func_0207328c(void*, int, int, void*);
extern void _ZN14BlendModelAnimD1Ev(void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int func_ov002_020aed18(int*);
extern int data_ov016_02114958[];
extern void func_020072c0(void);

int func_ov016_021111a0(void* c) {
    *(void**)c = data_ov016_02114958;
    func_0207328c((char*)c + 0x448, 7, 0xc, (void*)func_020072c0);
    _ZN14BlendModelAnimD1Ev((char*)c + 0x350);
    _ZN12WithMeshClsnD1Ev((char*)c + 0x190);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)c + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char*)c + 0x110);
    func_ov002_020aed18((int*)c);
    return (int)c;
}
