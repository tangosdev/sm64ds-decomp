//cpp
struct C;
typedef void (C::*PMF)();
struct TabEnt { PMF pmf; };
extern TabEnt data_ov015_021149ec[];
extern "C" {
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
}
struct C {
    char pad[0x330];
    int idx;
};
extern "C" int _ZN14KnockDownPlank8BehaviorEv(C *c)
{
    (c->*(data_ov015_021149ec[c->idx].pmf))();
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
