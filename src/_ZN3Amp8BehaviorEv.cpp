//cpp
extern "C" {
extern void func_ov070_02120d34(void *c);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, void *v);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void func_ov070_02120724(void *c);
extern int data_ov070_0212365c[];
int _ZN3Amp8BehaviorEv(char *c)
{
    int *p;
    func_ov070_02120d34(c);
    p = (int *)(((int)c + 0x414) & 0xFFFFFFFFFFFFFFFF);
    *p += data_ov070_0212365c[1];
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x1d8, c + 0x410);
    _ZN12CylinderClsn5ClearEv(c + 0x1d8);
    _ZN12CylinderClsn6UpdateEv(c + 0x1d8);
    func_ov070_02120724(c);
    return 1;
}
}
