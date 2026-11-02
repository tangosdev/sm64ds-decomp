#include "types.h"
#include "private/debugger_block.h"
struct Ring
{
    u16 head;
    u16 pad;
    u16 tail;
};


#pragma optimize_for_size on

/* Debug console byte sink, main-memory form: store val at the head of a
   0x4000-byte ring 0x8000 bytes into the buffer that the debugger block at
   0x027fff60 points to (+0x1c), then advance the head. The byte is dropped
   when the ring is full (next head == tail). The store is a u16
   read-modify-write. func_0206cbc0 picks it when flag 4 is set. */
void func_0206ca7c(struct Ring *r, int val)
{
    struct DebuggerBlock *sh = (struct DebuggerBlock *)0x027fff60;
    u16 head = r->head;
    u16 next = (head + 1) & 0x3fff;
    u16 tail = *(volatile u16 *)&r->tail;
    char *buf = sh->buf + 0x8000;
    int off;
    u16 cur;
    u16 v;
    if (next == tail)
        return;
    off = head & ~1;
    cur = *(u16 *)(buf + off);
    if ((head & 1) != 0)
        v = (cur & 0xff) | ((u16)val << 8);
    else
        v = (cur & 0xff00) | (u16)val;
    *(u16 *)(buf + off) = v;
    r->head = next;
}
