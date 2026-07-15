//cpp
extern "C" {
void _Z14ApproachLinearRiii(int *p, int t, int s);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned);
int _ZN9Animation8FinishedEv(void *);
int _ZN6Player12GetTalkStateEv(void *);
void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned);
void _ZN7Message7EndTalkEv(void);
void func_ov034_021125b8(void *, int);
}
extern void **data_ov034_0211389c[];
extern void **data_ov034_02113860[];

extern "C" void func_ov034_021120ac(char *c) {
    int i, k;
    char *anim;
    void *player;

    _Z14ApproachLinearRiii((int *)(c + 0x98), 0, 0x1000);

    if (*(unsigned char *)(c + 0x8da) == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov034_0211389c[0][1], 0x40000000, 0x1000, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x174, data_ov034_0211389c[1][1], 0x40000000, 0x1000, 0);
    }

    anim = c + 0x110;
    anim = anim + 0xc8;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64) {
        if (*(unsigned char *)(c + 0x8da) == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_0211389c[i][1], 0x40000000, 0x1000, 0);
    }

    if (_ZN9Animation8FinishedEv(c + 0x2f0)) {
{
int i; char *p;
p = c + 0x110;
i = 0;
        for (; i < 5; i++, p += 0x64)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p, data_ov034_02113860[i][1], 0, 0x1000, 0);
}
    }

    player = *(void **)(c + 0x8c8);

    {
        int flag; int i; char *p; int zero;
        p = c;
        flag = 1;
        zero = 0;
        i = 0;
        for (; i < 5; i++, p += 0x64) {
            if (*(int *)(p + 0x170) != (int)data_ov034_02113860[i][1])
                flag = zero;
        }
        if (flag == 1) {
            if (_ZN6Player12GetTalkStateEv(player) == 2) {
                _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
                _ZN7Message7EndTalkEv();
                func_ov034_021125b8(c, 3);
            }
        }
    }

    {
        unsigned char *t = (unsigned char *)(void *)(unsigned long long)(unsigned)(c + 0x8da);
        *t = (unsigned char)(*t + 1);
    }
    if (*(unsigned char *)(c + 0x8da) > 0xc)
        *(unsigned char *)(c + 0x8da) = 0xd;
}
