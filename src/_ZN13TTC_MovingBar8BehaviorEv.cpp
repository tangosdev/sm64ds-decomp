//cpp
typedef short s16;

extern "C" {
int _Z14ApproachLinearRsss(s16 *cur, s16 target, s16 step);
unsigned int RandomIntInternal(int *seed);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
void func_ov065_0211b40c(void *c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);

extern unsigned char data_0209f2c0;
extern s16 data_ov065_0211d334;
extern int data_0209e650;
}

class TTC_MovingBar {
public:
    int Behavior();
};

int TTC_MovingBar::Behavior() {
    void *c = (void *)this;

    switch (data_0209f2c0) {
    case 0:
    case 1:
        *(s16 *)((char *)c + 0x96) =
            (&data_ov065_0211d334)[data_0209f2c0];
        break;

    case 2:
        if (_Z14ApproachLinearRsss(
                (s16 *)((char *)c + 0x96),
                *(s16 *)((char *)c + 0x92), 0x32) != 0) {
            unsigned short r =
                (unsigned short)(RandomIntInternal(&data_0209e650) >> 16);

            *(s16 *)((char *)c + 0x92) =
                (s16)((r % 7) * 0xc8);

            if (r < 0x7fff)
                goto no_reverse;

            {
                s16 *p = (s16 *)(
                    ((int)c + 0x92) & 0xFFFFFFFFFFFFFFFFLL);
                *p *= (s16)-1;
            }
        }
    no_reverse:
        break;

    case 3:
        *(s16 *)((char *)c + 0x96) = 0;
        break;
    }

    {
        s16 *p = (s16 *)(
            ((int)c + 0x94) & 0xFFFFFFFFFFFFFFFFLL);
        *p = (s16)(*p + *(s16 *)((char *)c + 0x96));
    }

    *(s16 *)((char *)c + 0x8e) =
        *(s16 *)((char *)c + 0x94);

    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    func_ov065_0211b40c(c);

    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);

    return 1;
}
