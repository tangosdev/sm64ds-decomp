extern int func_0207328c(void *p, int a, int b, void *f);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ActorD1Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int data_ov027_021139d4[];
extern int func_020072c0[];
extern int _ZTV17ExclamationSwitch[];
extern int *data_020a0eac;
int _ZN11PyramidLiftD0Ev(char *c) {
    *(int**)(c) = data_ov027_021139d4;
    func_0207328c(c+0x37c, 0xa, 0xc, func_020072c0);
    _ZN5ModelD1Ev(c+0x320);
    *(int**)(c) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev(c+0x124);
    _ZN5ModelD1Ev(c+0xd4);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, data_020a0eac);
    return (int)c;
}
