typedef unsigned int u32;
typedef unsigned short u16;

typedef struct Node {
    void *unk0;
    struct Node *next;
    void *pool;
    u32 flags;
    int unk10;
    int unk14;
    u16 queue;
} Node;

typedef void (*Callback)(void *arg, int code);

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void func_0205c788(Node *self, int arg);
extern void OS_WakeupThread(u16 *self);
extern void func_0205d874(int *s);

void *func_0205d044(void *pool)
{
    unsigned char *c = (unsigned char *)pool;
    u32 irqSave = _ZN3IRQ7DisableEv();
    int f1 = (int)((*(volatile u32 *)(c + 0x10) & 0x20) != 0);

    if (f1 != 0) {
        u32 *p = (u32 *)(((int)(c + 0x10)) & 0xFFFFFFFFFFFFFFFFLL);
        *p &= ~0x20;

        Node *head = *(Node **)(c + 0x18);
        if (head != 0) {
            Node *node = head;
            do {
                Node *next = node->next;
                int flagset = (int)((*(volatile u32 *)&node->flags & 2) != 0);
                if (flagset != 0) {
                    if (*(Node **)(c + 0x18) == node)
                        *(Node **)(c + 0x18) = next;
                    func_0205c788(node, 3);
                    if (next == 0)
                        next = *(Node **)(c + 0x18);
                }
                node = next;
            } while (node != 0);
        }
    }

    {
        int f2 = (int)((*(volatile u32 *)(c + 0x10) & 8) != 0);
        if (f2 == 0) {
            Node *head2 = *(Node **)(c + 0x18);
            if (head2 != 0) {
                int wasSet = (int)((*(volatile u32 *)(c + 0x10) & 0x10) != 0);
                int notSet = (int)(wasSet == 0);
                if (notSet != 0) {
                    u32 *p = (u32 *)(((int)(c + 0x10)) & 0xFFFFFFFFFFFFFFFFLL);
                    *p |= 0x10;
                }
                _ZN3IRQ7RestoreEj(irqSave);
                if (notSet != 0) {
                    if (*(u32 *)(c + 0x4c) & 0x200) {
                        Callback cb = *(Callback *)(c + 0x48);
                        cb(head2, 9);
                    }
                }
                {
                    int nset = (int)((*(volatile u32 *)&head2->flags & 4) != 0);
                    if (nset != 0) {
                        OS_WakeupThread(&head2->queue);
                        return 0;
                    }
                }
                return head2;
            }
        }
    }

    {
        int f5 = (int)((*(volatile u32 *)(c + 0x10) & 0x10) != 0);
        if (f5 != 0) {
            u32 *p = (u32 *)(((int)(c + 0x10)) & 0xFFFFFFFFFFFFFFFFLL);
            *p &= ~0x10;
            if (*(u32 *)(c + 0x4c) & 0x400) {
                int buf[0x11];
                func_0205d874(buf);
                buf[2] = (int)c;
                {
                    Callback cb = *(Callback *)(c + 0x48);
                    cb(buf, 0xa);
                }
            }
        }
    }

    {
        int f7 = (int)((*(volatile u32 *)(c + 0x10) & 0x40) != 0);
        if (f7 != 0) {
            u32 *p = (u32 *)(((int)(c + 0x10)) & 0xFFFFFFFFFFFFFFFFLL);
            *p &= ~0x40;
            *p |= 8;
            OS_WakeupThread((u16 *)(c + 0xe));
        }
    }

    _ZN3IRQ7RestoreEj(irqSave);
    return 0;
}
