typedef unsigned char u8;
typedef unsigned short u16;
extern u8 data_ov002_02109dcc[];
extern int data_ov002_0210a6ec[];
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int a, int b, int c, unsigned int d);

void func_ov002_020d99a4(unsigned char *self)
{
    u8 idx;
    if (*(u8 *)(self + 0x6de) == 0) return;
    idx = *(u8 *)(self + 0x6e3);
    if (idx > 1) return;
    _ZN6Player7SetAnimEji5Fix12IiEj(self, data_ov002_0210a6ec[idx & 7], 0x40000000, 0x1000,
                                    (u16)(data_ov002_02109dcc[idx] + 1));
    *(u8 *)(((unsigned int)self + 0x6e3) & 0xFFFFFFFFFFFFFFFFULL) += 2;
    *(u8 *)(((unsigned int)self + 0x6e5) & 0xFFFFFFFFFFFFFFFFULL) |= 1;
}
