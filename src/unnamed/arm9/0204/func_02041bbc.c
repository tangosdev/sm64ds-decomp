extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);

typedef struct ListNode {
    struct ListNode* next;
} ListNode;

void func_02041bbc(ListNode** head, ListNode** insertSlot, ListNode* node)
{
    unsigned int saved = _ZN3IRQ7DisableEv();
    ListNode* cur = *head;
    if (cur == node)
    {
        *head = node->next;
        node->next = *insertSlot;
        *insertSlot = node;
    }
    else
    {
        while (cur != 0)
        {
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
