//cpp
struct SEnt { short a, b; };
extern "C" {
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
}
extern SEnt data_02082214[];
extern "C" int _ZN9LavaPlank8BehaviorEv(char *c)
{
    int val = *(unsigned short *)(c + 0x324) >> 4;
    *(int *)(c + 0x60) = data_02082214[val].a * (short)0x1e + *(int *)(c + 0x320);
    *(short *)(((int)c + 0x324)) += 0x400;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}
