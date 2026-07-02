extern void _ZN9Animation7AdvanceEv(void *);
extern int _ZNK9Animation12WillHitFrameEi(void *, int);
void func_ov079_021254b4(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN9Animation7AdvanceEv(c + 0x31c);
    if (!_ZNK9Animation12WillHitFrameEi(c + 0x31c, 0xf))
        return;
    *(int *)(c + 0x3b0) = 4;
    (*(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF)) |= 0x2000000;
}
