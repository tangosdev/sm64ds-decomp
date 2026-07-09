extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN9ActorBaseC1Ev(void *);
extern int data_0208e4b8[];
extern int data_02092680[];
extern int data_ov005_020c2490[];
int *func_ov005_020c21ec(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0xb0);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        p[0] = (int)data_0208e4b8;
        p[0] = (int)data_02092680;
        *(unsigned char *)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF) |= 1;
        *(unsigned char *)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF) |= 4;
        p[0] = (int)data_ov005_020c2490;
    }
    return p;
}
