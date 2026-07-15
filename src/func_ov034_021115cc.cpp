//cpp
extern "C" {
void _Z14ApproachLinearRiii(int *r, int b, int c);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned);
int _ZN9Animation8FinishedEv(void *);
void func_ov034_021125b8(void *, int);
}
extern void **data_ov034_0211389c[];
extern void **data_ov034_02113860[];

extern "C" void func_ov034_021115cc(char *self) {
    int i;
    int k;
    char *anim;

    _Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000);

    anim = self + 0x110;
    anim = anim + 0xc8;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64) {
        if (*(unsigned char *)(self + 0x8da) == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_0211389c[i][1], 0x40000000, 0x1000, 0);
    }

    if (_ZN9Animation8FinishedEv(self + 0x2f0)) {
{
int i; char *p;
p = self + 0x110;
i = 0;

        for (; i < 5; i++, p += 0x64) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p, data_ov034_02113860[i][1], 0, 0x1000, 0);
        }
        }

    }

    {
        int flag; int i; char *p; int zero;
        p = self;
        flag = 1;
        zero = 0;
        i = 0;
        for (; i < 5; i++, p += 0x64) {
            if (*(int *)(p + 0x170) != (int)data_ov034_02113860[i][1])
                flag = zero;
        }
        if (flag == 1)
            func_ov034_021125b8(self, 9);
    }

    {
        unsigned char *t = (unsigned char *)(void *)(unsigned long long)(unsigned)(self + 0x8da);
        *t = (unsigned char)(*t + 1);
    }
    if (*(unsigned char *)(self + 0x8da) > 0xc)
        *(unsigned char *)(self + 0x8da) = 0xd;
}
