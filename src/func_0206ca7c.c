typedef unsigned short u16;

struct Ring
{
    u16 head;
    u16 pad;
    u16 tail;
};

struct Shared
{
    char pad[0x1c];
    char *buf;
};

#pragma optimize_for_size on

void func_0206ca7c(struct Ring *r, int val)
{
    struct Shared *sh = (struct Shared *)0x027fff60;
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
