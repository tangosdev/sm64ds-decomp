//cpp
extern "C" {
extern void func_ov071_02122080(char *t);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char *self);
extern void func_ov071_02122414(char *c);
}

extern "C" int _ZN6Coffin8BehaviorEv(char *self)
{
    char *stateField;

    if ((*(int *)(stateField = self + 8) & 0xff) == 1) {
        func_ov071_02122080(self);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0)) {
            _ZN8Platform19UpdateClsnPosAndRotEv(self);
        }
        return 1;
    }

    {
        unsigned short *timer =
            (unsigned short *)(((int)self + 0x328) & 0xFFFFFFFFFFFFFFFFLL);
        *timer = *timer + 1;
    }
    func_ov071_02122414(self);
    func_ov071_02122080(self);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(self);
    }
    return 1;
}
