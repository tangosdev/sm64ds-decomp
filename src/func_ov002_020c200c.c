// NONMATCHING: push-set / frame (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;

extern int func_ov002_020c1e44(void* a, int x);
extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, void* st);
extern int _ZN6Player22IsBeingShotOutOfCannonEv(void* thiz);
extern void func_ov002_020eedc0(void* c, void* arg);
extern void func_ov002_020c1eb4(void* self, int arg);
extern char _ZN6Player7ST_DIVEE;
extern char _ZN6Player16ST_STOMACH_SLIDEE;
extern char _ZN6Player13ST_BUTT_SLIDEE;
extern char _ZN6Player12ST_LONG_JUMPE;
extern char _ZN6Player6ST_FLYE;

int func_ov002_020c200c(char* self, int arg)
{
    if (*(unsigned char*)(self + 0x70e) == 1)
        return func_ov002_020c1e44(self, arg);

    if (*(unsigned char*)(self + 0x709))
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &_ZN6Player7ST_DIVEE) ||
        _ZN6Player7IsStateERNS_5StateE(self, &_ZN6Player16ST_STOMACH_SLIDEE) ||
        _ZN6Player7IsStateERNS_5StateE(self, &_ZN6Player13ST_BUTT_SLIDEE) ||
        _ZN6Player7IsStateERNS_5StateE(self, &_ZN6Player12ST_LONG_JUMPE) ||
        _ZN6Player22IsBeingShotOutOfCannonEv(self) ||
        _ZN6Player7IsStateERNS_5StateE(self, &_ZN6Player6ST_FLYE))
    {
        if (*(int*)(self + 0x98) <= 0x10000)
            return 0;

        if (_ZN6Player22IsBeingShotOutOfCannonEv(self))
            func_ov002_020eedc0(self + 0x380, self);

        func_ov002_020c1eb4(self, arg);
        return 1;
    }

    return 0;
}
