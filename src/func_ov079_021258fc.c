typedef unsigned short u16;
typedef unsigned char u8;

extern int _ZN9Animation8FinishedEv(void *a);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int e);
extern int Vec3_HorzDist(const void *a, const void *b);
extern void _ZN9Animation7AdvanceEv(void *a);
extern void func_0201267c(unsigned int id, void *v);

extern void *data_ov079_021275ec[];
extern int data_ov079_021275dc[];
extern signed char data_0209f2f8;

void func_ov079_021258fc(char *self)
{
    unsigned int frame;
    int new_var;
    u8 idx;
    unsigned int q;
    void *e;
    unsigned int cur;
    unsigned int want;
    int lim;

    idx = *(u8 *)(self + 0x414);
    q = *(unsigned int *)(self + 0x324);
    e = data_ov079_021275ec[((int)idx * 5) + 3];
    cur = *(volatile unsigned int *)(self + 0x32c);
    if ((self && self) && self) {
    }
    want = *(unsigned int *)((char *)e + 4);
    frame = (unsigned short)(q >> 12);

    if (cur != want) {
        if (frame < 3) {
            *(int *)(self + 0x98) = 0;
        } else {
            new_var = idx;
            *(int *)(self + 0x98) = data_ov079_021275dc[new_var];
        }
        if (_ZN9Animation8FinishedEv(self + 0x31c) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                self + 0x2cc,
                *(void **)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * 5 + 3] + 4),
                0, 0x1000, 0);
        }
    } else {
        new_var = idx;
        *(int *)(self + 0x98) = data_ov079_021275dc[new_var];
    }

    if (data_0209f2f8 == 0x27)
        lim = 0xc8000;
    else
        lim = 0x2bc000;
    *(int *)(self + 0x328) = 0x1000;
    if (Vec3_HorzDist(self + 0x5c, self + 0x3bc) > lim) {
        int *p;
        *(int *)(self + 0x3b0) = 7;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x2cc,
            *(void **)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * 5 + 3] + 4),
            0, 0x1000, 0);
        p = (int *)(((int)self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p & ~0x2000000;
        goto after_dist;
    }

    if (*(int *)(self + 0x3f8) < 0x5dc000) {
        *(int *)(self + 0x98) = 0x9000;
        *(int *)(self + 0x328) = data_ov079_021275dc[*(u8 *)(self + 0x414)];
    }
    {
        int five = 5;
        if (*(int *)(self + 0x32c) == *(int *)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * five + 3] + 4)) {
            if (*(int *)(self + 0x3f8) < 0xc8000) {
                *(int *)(self + 0x3b0) = 3;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    self + 0x2cc,
                    *(void **)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * five] + 4),
                    0x40000000, 0x1000, 0);
            }
        }
    }
after_dist:
    if (*(int *)(self + 0x32c) == *(int *)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * 5 + 3] + 4)) {
        if (frame > 7) {
            if (frame < 0x1e)
                goto clear_flag;
            if (frame > 0x26)
                goto clear_flag;
        }
        *(int *)(self + 0x98) = 0;
        if (*(u8 *)(self + 0x40a) != 0)
            goto done_flag;
        *(u8 *)(self + 0x40a) = 1;
        func_0201267c(0xc5, self + 0x74);
        goto done_flag;
    clear_flag:
        *(u8 *)(self + 0x40a) = 0;
    done_flag:
        ;
        ;
        ;
    }

    _ZN9Animation7AdvanceEv(self + 0x31c);
}
