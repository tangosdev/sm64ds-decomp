typedef struct S6e06c {
    int unk0;
    unsigned int b4a:2;
    unsigned int b4b:3;
    unsigned int b4gap:2;
    unsigned int b4c:3;
    unsigned int b4rest:22;
    unsigned int b8:3;
    unsigned int b8rest:29;
    unsigned char fc;
    unsigned char fd;
} S6e06c;

void func_0206df14(void);
int func_0206df90(S6e06c *p, int a);

int func_0206e06c(S6e06c *p)
{
    if (p == 0) {
        func_0206df14();
        return;
    }

    if (p->fd != 0 || p->b4c == 0)
        return -1;

    if (p->b4b == 1)
        return 0;

    if (p->b8 >= 3)
        *(unsigned int *)(((int)p + 8) & 0xFFFFFFFFFFFFFFFF) = (*(unsigned int *)(((int)p + 8) & 0xFFFFFFFFFFFFFFFF) & ~7) | 2;

    if (p->b8 == 2)
        *(int *)((char *)p + 0x28) = 0;

    if (p->b8 != 1) {
        *(unsigned int *)(((int)p + 8) & 0xFFFFFFFFFFFFFFFF) &= ~7;
        return 0;
    }

    if (func_0206df90(p, 0) != 0) {
        p->fd = 1;
        *(int *)((char *)p + 0x28) = 0;
        return -1;
    }
    *(unsigned int *)(((int)p + 8) & 0xFFFFFFFFFFFFFFFF) &= ~7;
    *(int *)((char *)p + 0x18) = 0;
    *(int *)((char *)p + 0x28) = 0;
    return 0;
}
