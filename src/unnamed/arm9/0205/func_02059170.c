extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern void* func_02059364(void* a, void* n);
extern void* func_0205938c(void* next, void* node);
extern int* data_020a637c[];

struct Node { struct Node* prev; struct Node* next; int size; };
struct Bucket { int f0; struct Node* head; void* f8; };

void* func_02059170(int id, int idx2, int reqsize)
{
    int* pool;
    struct Bucket* b;
    struct Node* n;
    unsigned int state;
    int rem;

    state = _ZN3IRQ7DisableEv();
    pool = data_020a637c[id];
    if (pool == 0) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }
    if (idx2 < 0) idx2 = pool[0];
    b = (struct Bucket*)((char*)pool[4] + idx2 * 0xc);
    n = b->head;
    reqsize = ((int)((char*)reqsize + 0x20) + 0x1f) & ~0x1f;
    if (n != 0) {
        do {
            if (reqsize <= n->size) break;
            n = n->next;
        } while (n != 0);
    }
    if (n == 0) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }
    rem = n->size - reqsize;
    if ((unsigned int)rem < 0x40) {
        b->head = func_02059364(b->head, n);
    } else {
        struct Node* nn;
        n->size = reqsize;
        nn = (struct Node*)((char*)n + reqsize);
        nn->size = rem;
        nn->prev = n->prev;
        nn->next = n->next;
        if (nn->next != 0) nn->next->prev = nn;
        if (nn->prev != 0) nn->prev->next = nn;
        else b->head = nn;
    }
    b->f8 = func_0205938c(b->f8, n);
    _ZN3IRQ7RestoreEj(state);
    return (char*)n + 0x20;
}
