struct ListNode;
extern void func_02041bbc(struct ListNode **head, struct ListNode **insertSlot, struct ListNode *node);

struct ListNode *func_02041b60(char *base)
{
    struct ListNode *node;
    int *ctr;
    node = *(struct ListNode **)((base + 0x2000) + 0x700);
    func_02041bbc((struct ListNode **)(base + 0x2700), (struct ListNode **)(base + 0x2708), node);
    *(int *)((char *)node + 0x7c) = 1;
    ctr = (int *)(((long long)(int)(base + 0x2724)));
    *ctr += 1;
    return node;
}
