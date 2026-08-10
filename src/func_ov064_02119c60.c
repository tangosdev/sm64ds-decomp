extern void _Z14ApproachLinearRiii(int *p, int b, int c);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);

int func_ov064_02119c60(char *c)
{
    unsigned char *p = (unsigned char*)(c+0x380);
    *p -= 2;
    if (*(unsigned char*)(c+0x380) < 2)
        *(unsigned char*)(c+0x380) = 2;
    _Z14ApproachLinearRiii((int*)(c+0x80), 0x3000, 0x199);
    *(int*)(c+0x88) = *(int*)(c+0x80);
    *(int*)(c+0x84) = *(int*)(c+0x88);
    if (*(unsigned short*)(c+0x100) == 0 || *(int*)(c+0x80) >= 0x2ffd)
        _ZN9ActorBase18MarkForDestructionEv(c);
    return 1;
}
