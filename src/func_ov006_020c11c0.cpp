//cpp
extern "C" {
extern int _ZN9Animation8FinishedEv(void *anim);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *anim, void *file, int a, int b, int d, unsigned short e);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned id);
extern void func_ov006_020c1764(char *c);

void func_ov006_020c11c0(char *c)
{
    void *f7c = *(void **)(c + 0x7c);

    if (f7c == *(void **)(c + 0x214)) {
        if (_ZN9Animation8FinishedEv(c + 0x6c)) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x21c), 0, 0, 0x800, 0);
        }
    }

    f7c = *(void **)(c + 0x7c);
    if (f7c == *(void **)(c + 0x224) || f7c == *(void **)(c + 0x22c)) {
        if (_ZN9Animation8FinishedEv(c + 0x6c)) {
            *(int *)(c + 0x1d4) = 0;
            func_ov006_020c1764(c);
        }
        return;
    }

    if (f7c == *(void **)(c + 0x21c)) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x6c, 0)) {
            short cv = *(short *)(c + 0x100 + 0xde);
            short *p = (short *)(((long)c + 0x1de) & 0xFFFFFFFFFFFFFFFF);
            short old = *p;
            *p = old - 1;
            if (cv <= 0) {
                short state = *(short *)(c + 0x1a);
                if (state == 0) {
                    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x224), 0, 0x40000000, 0x800, 0);
                    if (*(int *)(c + 0x26c) != 0)
                        return;
                    _ZN5Sound12PlayBank2_2DEj(0x13a);
                    *(int *)(c + 0x26c) = 1;
                    return;
                } else if (state == 2) {
                    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x22c), 0, 0x40000000, 0x800, 0);
                    if (*(int *)(c + 0x26c) != 0)
                        return;
                    _ZN5Sound12PlayBank2_2DEj(0x13a);
                    *(int *)(c + 0x26c) = 1;
                    return;
                } else {
                    func_ov006_020c1764(c);
                    return;
                }
            }
        }

        if (*(int *)(c + 0x1d4) == 0) return;
        if (!_ZNK9Animation12WillHitFrameEi(c + 0x6c, 5)) return;
        _ZN5Sound12PlayBank2_2DEj(0x142);
        {
            short *p = *(short **)(c + 0x1d4);
            *p = (short)(*p + 1);
        }
        return;
    }

    if (f7c == *(void **)(c + 0x204)) {
        if (_ZN9Animation8FinishedEv(c + 0x6c)) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x214), 0, 0x40000000, 0x800, 0);
        }
    }
}
}
