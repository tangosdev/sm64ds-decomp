struct ListNode;
extern void func_02041bbc(struct ListNode **head, struct ListNode **insertSlot, struct ListNode *node);

void func_02041a94(char *base, char *node)
{
    int *ctr;
    func_02041bbc((struct ListNode **)(base + 0x2708), (struct ListNode **)(base + 0x2700), (struct ListNode *)node);
    ctr = (int *)(((long long)(int)(base + 0x2724)) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(node + 0x7c) = 0;
    *ctr -= 1;
}
