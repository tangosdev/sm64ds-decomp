extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void func_020584d0(void *node);
extern void func_02057f54(void);

typedef struct Node {
    char pad[0x68];
    struct Node *next;   /* field_68 */
    char pad2[0x70 - 0x68 - 4];
    int field_70;
} Node;

typedef struct {
    char pad[0xc];
    Node *head;          /* field_c */
} ListHead;

extern ListHead data_020a6134;
extern Node data_020a6188;

int func_02057e84(Node *target, int value)
{
    unsigned int saved;
    Node *cur;
    Node *prev;

    prev = 0;
    cur = data_020a6134.head;

    saved = _ZN3IRQ7DisableEv();

    while (cur != 0 && cur != target) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == 0 || cur == &data_020a6188) {
        _ZN3IRQ7RestoreEj(saved);
        return 0;
    }

    if (cur->field_70 != value) {
        if (prev == 0)
            data_020a6134.head = target->next;
        else
            prev->next = target->next;
        target->field_70 = value;
        func_020584d0(target);
        func_02057f54();
    }

    _ZN3IRQ7RestoreEj(saved);
    return 1;
}
