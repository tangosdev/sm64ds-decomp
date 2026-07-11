// Append b to a's child list: set b's owner, then link b at the head slot if
// empty, else at the end of the sibling chain. Reusing the parameter `a` as the
// list walker keeps the walker web pinned to r0 (forces the ROM's `mov r0,r2`).
struct N { char pad34[0x34]; struct N* owner; struct N* head; struct N* next; };

void func_ov007_020c10b0(struct N* a, struct N* b) {
    b->owner = a;
    if (a->head == 0) { a->head = b; return; }
    a = a->head;
    while (a->next != 0) a = a->next;
    a->next = b;
}
