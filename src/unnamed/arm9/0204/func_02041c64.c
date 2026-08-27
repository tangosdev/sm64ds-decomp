struct ListNode;
extern struct ListNode *func_02041b60(char *base);
extern void func_020423c8(void);

struct ListNode *func_02041c64(char *base, int a, int b)
{
    struct ListNode *node;
    node = *(struct ListNode **)((base + 0x2000) + 0x708);
    while (node != 0) {
        if (*(int *)((char *)node + 0x7c) == 1
            && *(int *)((char *)node + 0x8c) == a
            && *(int *)((char *)node + 0x90) == b) {
            break;
        }
        node = *(struct ListNode **)node;
    }
    if (node == 0) {
        node = func_02041b60(base);
        *(int *)((char *)node + 0x80) = 0;
        *(int *)((char *)node + 0x8c) = a;
        *(int *)((char *)node + 0x90) = b;
        func_020423c8();
    }
    return node;
}
