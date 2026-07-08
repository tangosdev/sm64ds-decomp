extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN9ActorBaseC1Ev(void *);

extern int _ZTV12ActorDerived[];
extern int _ZTV5Scene[];
extern int _ZTV14MultiBootScene[];
extern int _ZTV5Fader[];
extern int _ZTV15FaderBrightness[];
extern int _ZTV10FaderColor[];

typedef unsigned char u8;

void *func_020352b4(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x68);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        *(int **)p = _ZTV12ActorDerived;
        *(int **)p = _ZTV5Scene;
        {
            u8 *bp = (u8 *)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFFULL);
            *bp |= 1;
            *bp |= 4;
        }
        *(int **)p = _ZTV14MultiBootScene;
        {
            int *fp = (int *)(((int)p + 0x50) & 0xFFFFFFFFFFFFFFFFULL);
            fp[0] = (int)_ZTV5Fader;
            fp[0] = (int)_ZTV15FaderBrightness;
            fp[1] = 0x1000;
            fp[2] = 0;
            fp[0] = (int)_ZTV10FaderColor;
            *(short *)(fp + 3) = 0;
        }
    }
    return p;
}
