// NONMATCHING: base materialization / addressing (div=22). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;

extern int _Z14ApproachLinearRsss(s16 *cur, s16 target, s16 step);
extern unsigned int RandomIntInternal(int *seed);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
extern void func_ov065_0211b40c(void *c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);

extern unsigned char data_0209f2c0;
extern s16 data_ov065_0211d334;
extern int data_0209e650;

int _ZN13TTC_MovingBar8BehaviorEv(void *c) {
    int rv;

    switch (data_0209f2c0) {
    case 0:
    case 1:
        *(s16 *)((char *)c + 0x96) = (&data_ov065_0211d334)[data_0209f2c0];
        break;
    case 2:
        if (_Z14ApproachLinearRsss((s16 *)((char *)c + 0x96),
                                   *(s16 *)((char *)c + 0x92), 0x32) != 0) {
            rv = (int)(unsigned short)(RandomIntInternal(&data_0209e650) >> 16);
            *(s16 *)((char *)c + 0x92) = (s16)((rv % 7) * 0xc8);
            if ((unsigned int)rv >= 0x7fff) {
                s16 *p = (s16 *)((char *)c + 0x92);
                *p = (s16)(*p * (s16)-1);
            }
        }
        break;
    case 3:
        *(s16 *)((char *)c + 0x96) = 0;
        break;
    }

    {
        s16 *p = (s16 *)((char *)c + 0x94);
        *p = (s16)(*p + *(s16 *)((char *)c + 0x96));
    }
    *(s16 *)((char *)c + 0x8e) = *(s16 *)((char *)c + 0x94);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    func_ov065_0211b40c(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
