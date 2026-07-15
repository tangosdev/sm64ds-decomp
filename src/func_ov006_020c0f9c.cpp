//cpp
extern "C" {
extern int _ZNK9Animation12WillHitFrameEi(void *o, int f);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *o, void *bca, int a, int b, int fx, unsigned short t);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned a);
extern void func_ov006_020c1764(void *c);
extern int _ZN9Animation8FinishedEv(void *o);
}

extern "C" void func_ov006_020c0f9c(void *cc)
{
    char *c = (char*)cc;

    if (*(int*)(c + 0x7c) != *(int*)(c + 0x21c))
        goto other;

    if (_ZNK9Animation12WillHitFrameEi(c + 0x6c, 0)) {
        short cv = *(short*)(c + 0x100 + 0xde);
        short *p = (short*)(((long)c + 0x1de) & 0xFFFFFFFFFFFFFFFF);
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            int a;
            *(int*)(c + 0x1d4) = 0;
            a = *(short*)(c + 0x1a);
            if (a == 0) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void**)(c + 0x224), 0, 0x40000000, 0x800, 0);
                if (*(int*)(c + 0x26c) != 0)
                    return;
                _ZN5Sound12PlayBank2_2DEj(0x13a);
                *(int*)(c + 0x26c) = 1;
                return;
            } else if (a == 2) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void**)(c + 0x22c), 0, 0x40000000, 0x800, 0);
                if (*(int*)(c + 0x26c) != 0)
                    return;
                _ZN5Sound12PlayBank2_2DEj(0x13a);
                *(int*)(c + 0x26c) = 1;
                return;
            } else {
                func_ov006_020c1764(c);
                return;
            }
        }
    }

    if (*(int*)(c + 0x1d4) == 0)
        return;
    if (!_ZNK9Animation12WillHitFrameEi(c + 0x6c, 5))
        return;
    _ZN5Sound12PlayBank2_2DEj(0x142);
    *(short*)*(char**)(c + 0x1d4) += 1;
    return;

other:
    if (*(int*)(c + 0x7c) == *(int*)(c + 0x224) || *(int*)(c + 0x7c) == *(int*)(c + 0x22c)) {
        if (_ZN9Animation8FinishedEv(c + 0x6c))
            func_ov006_020c1764(c);
    }
}
