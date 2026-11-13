#include "types.h"
extern void _ZN3IRQ11DisableIRQsEj(u32 mask);
extern u64 func_02059650(void);
extern void func_02059c18(void *p);
extern void func_02059a60(void *p, u64 key);

#ifdef _MSC_VER
/* The ROM's 4-byte layout for the 64-bit field(s) of the alarm record (see
 * src/func_02059a60.c); mwccarm never sees this arm. */
#pragma pack(push, 4)
#endif
struct Node {
    void (*f0)(int);   /* 0x0 */
    int f4;            /* 0x4 */
    int f8;
    u64 deadline;      /* 0xc, 0x10 */
    struct Node *prev; /* 0x14 */
    struct Node *next; /* 0x18 */
    u64 key;           /* 0x1c, 0x20 */
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif
struct List {
    struct Node *head;
    struct Node *tail;
};
extern struct List data_020a6444;
extern int data_023c0000;

void func_02059834(void)
{
    struct Node *e;
    void (*cb)(int);
    struct Node *n;
    u64 now;
    *(volatile unsigned short *)0x4000106 = 0;
    _ZN3IRQ11DisableIRQsEj(0x10);
    *(int *)(((int)&data_023c0000 + 0x3ff8)) |= 0x10;
    now = func_02059650();
    e = data_020a6444.head;
    if (e == 0) return;
    if (now < e->deadline) {
        func_02059c18(e);
        return;
    }
    n = e->next;
    data_020a6444.head = n;
    if (n == 0) {
        data_020a6444.tail = 0;
    } else {
        n->prev = 0;
    }
    cb = e->f0;
    if (e->key == 0) e->f0 = 0;
    if (cb != 0) cb(e->f4);
    if (e->key != 0) {
        e->f0 = cb;
        func_02059a60(e, 0);
    }
    if (data_020a6444.head != 0) func_02059c18(data_020a6444.head);
}
