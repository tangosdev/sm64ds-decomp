typedef struct Block {
    struct Block *prev;
    struct Block *next;
    int size;
} Block;

void *func_0205929c(void *ap, void *bp)
{
    Block *prev = 0;
    Block *cur = (Block *)ap;
    Block *b = (Block *)bp;

    while (cur != 0) {
        if (b <= cur)
            break;
        prev = cur;
        cur = cur->next;
    }

    b->next = cur;
    b->prev = prev;
    if (cur != 0) {
        cur->prev = b;
        if ((Block *)((char *)b + b->size) == cur) {
            int *bsize = (int *)(((long long)(int)&b->size) & 0xFFFFFFFFFFFFFFFFLL);
            *bsize += cur->size;
            cur = cur->next;
            b->next = cur;
            if (cur != 0)
                cur->prev = b;
        }
    }
    if (prev != 0) {
        prev->next = b;
        if ((Block *)((char *)prev + prev->size) == b) {
            int *psize = (int *)(((long long)(int)&prev->size) & 0xFFFFFFFFFFFFFFFFLL);
            *psize += b->size;
            prev->next = cur;
            if (cur != 0)
                cur->prev = prev;
        }
        return ap;
    }
    return b;
}
