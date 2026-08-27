extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
struct Node;
extern void func_02059c18(struct Node *p);

struct Node {
    int f0;
    int pad[4];
    struct Node *prev;   /* 0x14 */
    struct Node *next;   /* 0x18 */
    int f1c;             /* 0x1c */
    int f20;             /* 0x20 */
};

struct List {
    struct Node *head;
    struct Node *tail;
};

extern struct List data_020a6444;

void func_02059950(struct Node *thiz)
{
    int saved = _ZN3IRQ7DisableEv();
    struct Node *next;
    if (thiz->f0 == 0) {
        _ZN3IRQ7RestoreEj(saved);
        return;
    }
    next = thiz->next;
    if (next == 0)
        data_020a6444.tail = thiz->prev;
    else
        next->prev = thiz->prev;
    if (thiz->prev != 0) {
        thiz->prev->next = next;
    } else {
        data_020a6444.head = next;
        if (next != 0)
            func_02059c18(next);
    }
    thiz->f0 = 0;
    thiz->f1c = 0;
    thiz->f20 = 0;
    _ZN3IRQ7RestoreEj(saved);
}
