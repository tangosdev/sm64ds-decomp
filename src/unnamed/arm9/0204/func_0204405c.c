/* func_0204405c at 0x0204405c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct N {
    struct N *prev;
    struct N *next;
    int pad8;
    volatile unsigned short c;
};
struct L {
    struct N *head;
    struct N *tail;
};

extern int func_0203b244(struct L *l, struct N *n);
extern int func_0203b2ec(struct L *l, struct N *n, struct N *p);

int func_0204405c(struct L *l, struct N *n)
{
    struct N *node;
    struct N *next;
    node = l->head;
    if (n == 0)
        return 0;
    if (node == 0)
        return func_0203b244(l, n);
    if (node->c > n->c)
        return func_0203b2ec(l, n, 0);
    goto enter;
advance:
    node = next;
enter:
    next = node->next;
    if (next == 0)
        goto done;
    if (next->c <= n->c)
        goto advance;
done:
    return func_0203b2ec(l, n, node);
}
