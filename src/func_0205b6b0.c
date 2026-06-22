extern void _ZN4CP1514FlushDataCacheEjj(unsigned int addr, unsigned int size);

void func_0205b6b0(void *thiz)
{
    int *node = *(int **)((char *)thiz + 0x18);
    if (node == 0)
        return;
    do {
        int *next = (int *)node[1];
        node[0] = 0;
        node[1] = 0;
        _ZN4CP1514FlushDataCacheEjj((unsigned int)node, 8);
        node = next;
    } while (node != 0);
}
