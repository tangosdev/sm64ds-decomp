extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);

int func_ov102_0214d114(char *c)
{
    *(short *)(((int)c + 0x8e)) += 0x1000;
    _ZN5dCc_c5ClearEv(c + 0x378);
    _ZN5dCc_c6UpdateEv(c + 0x378);
    return 1;
}
