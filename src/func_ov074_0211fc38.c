extern int ApproachAngle(void* p, int a, int b, int c, int d);
extern int _ZN9Animation8FinishedEv(void* a);
extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* p, void* a, int b);
extern void _ZN7Message11PrepareTalkEv(void);
extern int func_ov074_02121a20(void* c, int idx);
extern void func_ov074_02121a4c(void* c, int i);
extern void func_ov074_021203e4(char* c, int i);
extern void func_ov074_021216f4(void* c);

void func_ov074_0211fc38(char* c)
{
    if (ApproachAngle(c + 0x5f6, 0, 0xa, 0x100, 1) != 0) goto reset;
    if (_ZN9Animation8FinishedEv(c + 0x260) == 0) return;
    if (*(unsigned char*)(c + 0x604) != 0) goto detox;
    {
        void* player = _ZN5Actor13ClosestPlayerEv(c);
        if (_ZN6Player9StartTalkER9ActorBaseb(player, c, 1) == 0) return;
        _ZN7Message11PrepareTalkEv();
        if (func_ov074_02121a20(c, 1) != 0 || func_ov074_02121a20(c, 2) != 0) {
            func_ov074_02121a4c(c, 0xc);
            return;
        }
        func_ov074_021203e4(c, 7);
        *(void**)(c + 0x5c8) = player;
        return;
    }
detox:
    func_ov074_021203e4(c, 6);
    return;
reset:
    func_ov074_021216f4(c);
}
