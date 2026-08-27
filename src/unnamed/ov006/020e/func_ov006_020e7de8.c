extern int _ZN9Animation8FinishedEv(void *anim);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern void func_ov006_020e7f5c(void *o);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void func_ov006_020e7940(char *p, short v);
extern void func_ov006_020e794c(int *p, int v);
extern void func_ov006_020e792c(char *c, short v);

void func_ov006_020e7de8(char *c)
{
    if (_ZN9Animation8FinishedEv(c + 0x5c)) {
        if (*(int *)(c + 0x20c) == 0)
            _ZN5Sound12PlayBank2_2DEj(0x18f);
        func_ov006_020e7f5c(c);
        return;
    }
    if (_ZNK9Animation12WillHitFrameEi(c + 0x5c, 0x1e) == 0)
        return;
    func_ov006_020e7940((char *)(c + 0x84), 0x1e);
    func_ov006_020e794c((int *)(c + 0x84), 0x800);
    func_ov006_020e792c((char *)(c + 0x84), -1);
    *(int *)(c + 0x208) = 1;
}
