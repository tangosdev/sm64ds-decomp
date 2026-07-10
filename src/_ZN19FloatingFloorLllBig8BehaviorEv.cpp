//cpp
struct SEnt { short a, b; };
extern "C" {
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
}
extern SEnt data_02082214[];
extern "C" int _ZN19FloatingFloorLllBig8BehaviorEv(char *c)
{
    int val = *(unsigned short *)(c + 0x324) >> 4;
    *(int *)(c + 0x60) = data_02082214[val].a * (short)0x1e + *(int *)(c + 0x320);
    *(short *)(((int)c + 0x324) & 0xFFFFFFFFFFFFFFFF) += 0x400;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
