extern void func_0207328c(void *arr, int a, int b, void *fn);
extern void _ZN18MovingMeshColliderD1Ev(void *c);
extern void _ZN5ModelD1Ev(void *c);
extern void _ZN5ActorD1Ev(void *c);
extern int data_ov064_0211be10[];
extern void _ZN19CylinderClsnWithPosD1Ev(void *c);
extern int data_ov002_0210ae38[];
int func_ov064_02118138(char *c)
{
    *(int**)c = (int*)data_ov064_0211be10;
    func_0207328c(c + 0x360, 8, 0x3c, (void*)_ZN19CylinderClsnWithPosD1Ev);
    *(int**)c = (int*)data_ov002_0210ae38;
    _ZN18MovingMeshColliderD1Ev(c + 0x124);
    _ZN5ModelD1Ev(c + 0xd4);
    _ZN5ActorD1Ev(c);
    return (int)c;
}
