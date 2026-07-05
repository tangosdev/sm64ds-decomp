struct Actor;
extern struct Actor *func_0200e55c(unsigned int ownerID);

int func_020080b0(char *c)
{
    struct Actor *obj = func_0200e55c(0x13);
    char *base = (char *)(((long long)(int)((char *)obj + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);

    int z = *(int *)(base + 8);
    int y = *(int *)(base + 4) + *(int *)(c + 0x170);
    int x = *(int *)(base + 0);

    *(int *)(c + 0x80) = x;
    *(int *)(c + 0x84) = y;
    *(int *)(c + 0x88) = z;
    return 1;
}
