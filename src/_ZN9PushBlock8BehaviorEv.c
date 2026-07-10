extern void func_ov002_020b9750(char* c);
extern void func_ov002_020b979c(char* c);
extern void func_ov002_020b9a1c(char* c);
extern void func_ov002_020b993c(char* c);
extern void _ZN12CylinderClsn5ClearEv(char* t);
extern void _ZN12CylinderClsn6UpdateEv(char* t);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void _ZN5Actor13SmallPoofDustEv(char* c);
extern void _ZN9ActorBase18MarkForDestructionEv(char* c);

int _ZN9PushBlock8BehaviorEv(char* c)
{
    int b = (int)((*(int*)(c + 0xb0) & 0x40000) != 0);
    if (b != 0) return 1;
    *(int*)(c + 0x80) = 0xfa0;
    *(int*)(c + 0x84) = 0xfa0;
    *(int*)(c + 0x88) = 0xfa0;
    func_ov002_020b9750(c);
    func_ov002_020b979c(c);
    func_ov002_020b9a1c(c);
    func_ov002_020b993c(c);
    _ZN12CylinderClsn5ClearEv(c + 0x1cc);
    _ZN12CylinderClsn6UpdateEv(c + 0x1cc);
    if (_ZN8SaveData16HasPlayerLostCapEv()) {
        _ZN5Actor13SmallPoofDustEv(c);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
    return 1;
}
