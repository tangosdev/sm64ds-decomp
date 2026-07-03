extern void _ZN9ActorBase18MarkForDestructionEv(void *);

void func_ov092_02130fcc(char *c)
{
    int limit;
    int cur;

    cur = *(int *)(((long long)(int)((char *)c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
    cur -= 0x5000;
    *(int *)(((long long)(int)((char *)c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) = cur;
    limit = *(int *)(c + 0x55c) - 0x3e8000;
    cur = *(int *)(c + 0x60);
    if (cur >= limit)
        return;
    _ZN9ActorBase18MarkForDestructionEv(c);
}