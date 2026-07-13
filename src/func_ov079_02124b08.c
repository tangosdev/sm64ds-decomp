typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_ov079_02124ed4(void *self);
extern void func_ov079_02124dec(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *file, int a, int b, unsigned int u);
extern void _ZN9Animation7AdvanceEv(void *a);
extern int _ZN9Animation8FinishedEv(void *a);
extern void func_020393c4(void *p, int v);

extern void *data_ov079_021275ec[];
extern int data_ov079_02127cfc[];

void func_ov079_02124b08(char *self)
{
    u8 st = *(u8 *)(self + 0x40c);

    if (st < 0xa) {
        *(int *)(self + 0x98) = 0;
        *(int *)(self + 0x3c8) = 0;
        *(int *)(self + 0x3cc) = 0;
        *(int *)(self + 0x3d0) = 0;
        if (*(u8 *)(self + 0x414) != 0)
            func_ov079_02124ed4(self);
        else
            func_ov079_02124dec(self);

        {
            int lim = *(u8 *)(self + 0x414) * 0x14 + 0x64;
            u16 cur = *(u16 *)(self + 0x100);
            if ((int)cur <= lim) {
                if (*(u8 *)(self + 0x407) != 1)
                    return;
                if (cur <= 0x1e)
                    return;
            }
            *(u8 *)(self + 0x407) = 0;
            *(u8 *)(self + 0x40c) = 0xb;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                self + 0x2cc,
                *(void **)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * 5 + 2] + 4),
                0x40000000, 0x1000, 0);
            {
                char *base = self + 0x300;
                *(u16 *)(base + 0xfc) = 0;
            }
            return;
        }
    }

    
    if (st == 0xa) {
        _ZN9Animation7AdvanceEv(self + 0x31c);
        {
            u16 *p = (u16 *)(((int)self + 0x3fc) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        if (*(u16 *)(self + 0x300 + 0xfc) <= 0x1e)
            return;
        *(u8 *)(self + 0x407) = 0;
        {
            u8 *p = (u8 *)(((int)self + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x2cc,
            *(void **)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * 5 + 2] + 4),
            0x40000000, 0x1000, 0);
        {
            char *base = self + 0x300;
            *(u16 *)(base + 0xfc) = 0;
        }
        return;
    }

    if (*(s16 *)(self + 0x8c) > 0) {
        char *base = self + 0x300;
        if (*(u16 *)(base + 0xfc) >= 0xc) {
            s16 *ang = (s16 *)(((int)self + 0x8c) & 0xFFFFFFFFFFFFFFFFLL);
            *(int *)(self + 0x3c8) = -0x290;
            *ang = (s16)(*ang + *(int *)(self + 0x3c8));
        }
        {
            u16 *p = (u16 *)(((int)self + 0x3fc) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
    } else {
        int z = 0;
        *(int *)(self + 0x3c8) = z;
        *(s16 *)(self + 0x8c) = z;
        if (_ZN9Animation8FinishedEv(self + 0x31c) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                self + 0x2cc,
                *(void **)((char *)data_ov079_021275ec[*(u8 *)(self + 0x414) * 5 + 4] + 4),
                0x40000000, 0x1000, 0);

            if (*(u8 *)(self + 0x414) == 0) {
                func_020393c4((char *)(((int)self + 0x418) & 0xFFFFFFFFFFFFFFFFLL), 0);
            }
            *(u8 *)(self + 0x403) = 0;
            *(u8 *)(self + 0x402) = 0;
            if (*(u8 *)(self + 0x40b) != 0) {
                *(int *)(self + 0x3b0) = *(int *)(self + 0x3b4);
                *(u8 *)(self + 0x40b) = 0;
                if (*(int *)(self + 0x3b0) == 7)
                    *(int *)(self + 0x3b4) = 0xb;
                return;
            }
            if (*(u8 *)(self + 0x414) != 0)
                *(int *)(self + 0x3b0) = 2;
            else
                *(int *)(self + 0x3b0) = 1;
            return;
        }
    }

    _ZN9Animation7AdvanceEv(self + 0x31c);
    {
        u8 v;
        unsigned int q;
        int m;
        v = *(u8 *)(self + 0x414);
        q = *(unsigned int *)(self + 0x324);
        m = v * 0x30;
        *(int *)(self + 0x410) = data_ov079_02127cfc[m + (int)((unsigned short)(q >> 12))];
    }
}
