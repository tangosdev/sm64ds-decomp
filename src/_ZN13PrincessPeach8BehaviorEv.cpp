//cpp
extern "C" {
extern int func_ov085_0212a430(void *c);
extern int func_ov085_02129dbc(void *c);
extern void _ZN9Animation7AdvanceEv(void *a);
extern void _ZN12CylinderClsn5ClearEv(void *);
extern void _ZN12CylinderClsn6UpdateEv(void *);
extern int func_ov085_02129fdc(void *c);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); };
extern "C" {
int _ZN13PrincessPeach8BehaviorEv(char *c)
{
    func_ov085_0212a430(c);
    func_ov085_02129dbc(c);
    if (*(int*)(c + 0x354) != 1)
        _ZN9Animation7AdvanceEv(c + 0x124);
    ((Sub*)(c + 0xd4))->g3();
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    _ZN12CylinderClsn6UpdateEv(c + 0x160);
    func_ov085_02129fdc(c);
    return 1;
}
}
