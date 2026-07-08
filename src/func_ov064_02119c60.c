extern int ApproachLinear(int *v, int a, int b);
extern void MarkForDestruction(void *self);

int func_ov064_02119c60(char *self) {
    *(unsigned char *)(((long long)(int)(self + 0x380)) & 0xFFFFFFFFFFFFFFFFLL) -= 2;
    if (*(unsigned char *)(self + 0x380) < 2)
        *(unsigned char *)(self + 0x380) = 2;
    ApproachLinear((int *)(self + 0x80), 0x3000, 0x199);
    *(int *)(self + 0x88) = *(int *)(self + 0x80);
    *(int *)(self + 0x84) = *(int *)(self + 0x88);
    if (*(unsigned short *)(self + 0x100) == 0 || *(int *)(self + 0x80) >= 0x2ffd)
        MarkForDestruction(self);
    return 1;
}
