extern void _ZN9Animation7AdvanceEv(char *anim);
extern int _ZN9Animation8FinishedEv(char *anim);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void func_ov080_02124360(char *self);
extern int data_ov080_0212767c[];

void func_ov080_02123860(char *self)
{
    unsigned int idx;

    _ZN9Animation7AdvanceEv(self + 0x124);
    idx = (unsigned int)(*(int *)(self + 0x12c) << 4) >> 0x10;
    if (idx >= 0xf) {
        *(int *)((int)(self + 0x150) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        *(int *)((int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x10000000;
        if (idx == 0xf) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2a, *(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2b, *(int *)(self + 0x5c), *(int *)(self + 0x60) + 0x1e000, *(int *)(self + 0x64));
        }
    }
    {
        int raw = *(int *)(self + 0x12c);
        int *tbl = (int *)(int)(((long long)(int)data_ov080_0212767c) & 0xFFFFFFFFFFFFFFFFLL);
        idx = (unsigned int)(raw << 4) >> 0x10;
        *(int *)(self + 0x140) = tbl[idx] << 0xc;
    }
    if (_ZN9Animation8FinishedEv(self + 0x124) == 0)
        return;
    *(int *)(self + 0x17c) = 1;
    func_ov080_02124360(self);
}
