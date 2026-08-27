typedef unsigned long long u64;

struct Node {
    char pad0[0xc];
    u64 key;        /* 0x0c */
    struct Node *prev; /* 0x14 */
    struct Node *next; /* 0x18 */
    u64 step;       /* 0x1c */
    u64 base;       /* 0x24 */
};

extern u64 func_02059650(void);
extern void func_02059c18(struct Node *p);
extern struct Node *data_020a6444[2];

void func_02059a60(struct Node *p, u64 key)
{
    struct Node *n;

    if (p->step != 0) {
        u64 t = func_02059650();
        key = p->base;
        if (key < t) {
            key = key + ((t - key) / p->step + 1) * p->step;
        }
    }
    p->key = key;

    n = data_020a6444[0];
    if (n != 0) {
        do {
            if ((long long)(key - n->key) < 0) {
                p->prev = n->prev;
                n->prev = p;
                p->next = n;
                if (p->prev) {
                    p->prev->next = p;
                    return;
                }
                data_020a6444[0] = p;
                func_02059c18(p);
                return;
            }
            n = n->next;
        } while (n);
    }

    p->next = 0;
    {
        struct Node *tail = data_020a6444[1];
        data_020a6444[1] = p;
        p->prev = tail;
        if (tail) {
            tail->next = p;
            return;
        }
    }
    data_020a6444[1] = p;
    data_020a6444[0] = p;
    func_02059c18(p);
}
