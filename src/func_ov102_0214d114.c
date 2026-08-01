extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);

int func_ov102_0214d114(char *c)
{
    *(short *)(((int)c + 0x8e)) += 0x1000;
    _ZN12CylinderClsn5ClearEv(c + 0x378);
    _ZN12CylinderClsn6UpdateEv(c + 0x378);
    return 1;
}
