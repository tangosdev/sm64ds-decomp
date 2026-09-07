#include "types.h"
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
typedef struct Node {
    u32 f0;
    struct Node *next;
    struct Node *prev;
    u32 pad0c;
    u32 flags;
} Node;

typedef struct Mgr {
    Node *cur;
    u16 a;
    u16 b;
    u32 c;
} Mgr;

extern Mgr data_020a804c;
extern u32 data_020a8048;

void func_0205cbe4(Node *node)
{
    u32 saved;
    Node *n;
    if (node->f0 == 0) return;
    saved = _ZN3IRQ7DisableEv();
    n = node->next;
    if (n) n->prev = node->prev;
    n = node->prev;
    if (n) n->next = node->next;
    node->f0 = 0;
    node->prev = 0;
    node->next = node->prev;
    *(u32 *)((u32)node + 0x10) &= ~1u;
    if (data_020a804c.cur == node) {
        data_020a804c.c = 0;
        data_020a804c.b = 0;
        data_020a804c.cur = (Node *)data_020a8048;
        data_020a804c.a = 0;
    }
    _ZN3IRQ7RestoreEj(saved);
}
