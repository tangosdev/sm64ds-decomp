struct ListNode;
extern void func_02041bbc(struct ListNode **head, struct ListNode **insertSlot, struct ListNode *node);

void func_02041ae8(char *base, char *node)
{
    func_02041bbc((struct ListNode **)(base + 0x2704), (struct ListNode **)(base + 0x2708), (struct ListNode *)node);
    *(int *)(node + 0x7c) = 3;
}
