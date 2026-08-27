extern int _ZNK9Animation13GetFrameCountEv(void *self);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int frame);
extern void func_ov062_02117994(char *c, int idx);
void func_ov062_0211811c(char *c) {
    char *b = c + 0x300;
    if (*(unsigned short*)(b + 0xc6) != 0) {
        unsigned short *p = (unsigned short*)(((int)c + 0x3c6));
        *p = (unsigned short)(*p - 1);
        return;
    }
    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, (unsigned short)(_ZNK9Animation13GetFrameCountEv(c + 0x350) - 1)) == 0)
        return;
    {
        unsigned short *q = (unsigned short*)(((int)c + 0x3c4));
        *q = (unsigned short)(*q + 1);
    }
    func_ov062_02117994(c, 2);
}
