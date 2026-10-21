extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);

typedef struct ListNode {
    struct ListNode *next;
} ListNode;

void func_02041bbc(ListNode **head, ListNode **insertSlot, ListNode *node) {
    unsigned int saved = _ZN3IRQ7DisableEv();
    ListNode *cur = *head;
    if (cur == node) {
        *head = node->next;
        node->next = *insertSlot;
        *insertSlot = node;
    } else {
        while (cur != 0) {
            if (cur->next != node)
                break;
            cur->next = node->next;
            node->next = *insertSlot;
            *insertSlot = node;
            break;
        }
    }
    _ZN3IRQ7RestoreEj(saved);
}

struct ListNode;
extern void func_02041bbc(struct ListNode **head, struct ListNode **insertSlot,
                          struct ListNode *node);

struct ListNode *func_02041b60(char *base) {
    struct ListNode *node;
    int *ctr;
    node = *(struct ListNode **)((base + 0x2000) + 0x700);
    func_02041bbc((struct ListNode **)(base + 0x2700), (struct ListNode **)(base + 0x2708), node);
    *(int *)((char *)node + 0x7c) = 1;
    ctr = (int *)(base + 0x2724);
    *ctr += 1;
    return node;
}

struct ListNode;
extern void func_02041bbc(struct ListNode **head, struct ListNode **insertSlot,
                          struct ListNode *node);

void func_02041b24(char *base, char *node) {
    func_02041bbc((struct ListNode **)(base + 0x2708), (struct ListNode **)(base + 0x2704),
                  (struct ListNode *)node);
    *(int *)(node + 0x7c) = 2;
}

struct ListNode;
extern void func_02041bbc(struct ListNode **head, struct ListNode **insertSlot,
                          struct ListNode *node);

void func_02041ae8(char *base, char *node) {
    func_02041bbc((struct ListNode **)(base + 0x2704), (struct ListNode **)(base + 0x2708),
                  (struct ListNode *)node);
    *(int *)(node + 0x7c) = 3;
}

struct ListNode;
extern void func_02041bbc(struct ListNode **head, struct ListNode **insertSlot, struct ListNode *node);

void func_02041a94(char *base, char *node)
{
    int *ctr;
    func_02041bbc((struct ListNode **)(base + 0x2708), (struct ListNode **)(base + 0x2700), (struct ListNode *)node);
    ctr = (int *)(base + 0x2724);
    *(int *)(node + 0x7c) = 0;
    *ctr -= 1;
}
