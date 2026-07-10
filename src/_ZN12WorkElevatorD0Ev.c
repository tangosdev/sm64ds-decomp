extern int func_0207328c(void *p, int a, int b, void *f);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN5ActorD1Ev(void *p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern int func_ov075_0211478c[];
extern int _ZTV17ExclamationSwitch[];
extern int *data_020a0eac;
int _ZN12WorkElevatorD0Ev(char *c) {
    *(int**)(c) = func_ov075_0211478c;
    func_0207328c(c+0x520, 4, 0x1c8, _ZN18MovingMeshColliderD1Ev);
    func_0207328c(c+0x320, 4, 0x50, _ZN5ModelD1Ev);
    *(int**)(c) = _ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev(c+0x124);
    _ZN5ModelD1Ev(c+0xd4);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, data_020a0eac);
    return (int)c;
}
