
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned);
int _ZN9Animation8FinishedEv(void *);
void func_ov034_021125b8(void *, int);
extern void **data_ov034_02113888[];
extern void **data_ov034_02113874[];

void func_ov034_021113d4(char *thiz) {
    int i;
    int k;
    char *anim;
    anim = thiz + 0x110;
    anim = anim + 0xc8;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64) {
        if (*(unsigned char *)(thiz + 0x8da) == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_02113888[i][1], 0x40000000, 0x1000, 0);
    }
    {
        int idx; char *fin; char *an;
        fin = thiz + 0x160; idx = 0; an = thiz + 0x110;
        for (; idx < 5; idx++, fin += 0x64, an += 0x64) {
            if (_ZN9Animation8FinishedEv(fin))
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(an, data_ov034_02113874[idx][1], 0, 0x1000, 0);
        }
    }
    {
int flag; int i; char *p; int zero;
p = thiz;
flag = 1;
zero = 0;
i = 0;

        for (; i < 5; i++, p += 0x64) {
            if (*(int *)(p + 0x170) != (int)data_ov034_02113874[i][1])
                flag = zero;
        }
        if (flag == 1)
            func_ov034_021125b8(thiz, 4);
    }
    {
        unsigned char *t = (unsigned char *)(void *)(unsigned long long)(unsigned)(thiz + 0x8da);
        *t = (unsigned char)(*t + 1);
    }
    if (*(unsigned char *)(thiz + 0x8da) > 0xc)
        *(unsigned char *)(thiz + 0x8da) = 0xd;
}
