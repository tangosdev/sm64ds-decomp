extern int func_0207328c(void *p, int a, int b, void *f);
extern int _ZN5ModelD1Ev(void *p);
extern int _ZN18MovingMeshColliderD1Ev(void *p);
extern int _ZN5ActorD1Ev(void *p);
extern int data_ov002_0210af70[];
extern int data_ov002_0210ae38[];
int _ZN8PathLiftD2Ev(char *c) {
    *(int**)(c) = data_ov002_0210af70;
    func_0207328c(c+0x320, 3, 0x50, _ZN5ModelD1Ev);
    *(int**)(c) = data_ov002_0210ae38;
    _ZN18MovingMeshColliderD1Ev(c+0x124);
    _ZN5ModelD1Ev(c+0xd4);
    _ZN5ActorD1Ev(c);
    return (int)c;
}
