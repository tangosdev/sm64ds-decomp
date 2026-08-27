struct Inner { char pad[0xc]; unsigned short id; };
struct Node { char pad[4]; struct Node* next; struct Inner* inner; };

struct Node* func_02043f4c(struct Node** pp, int key, struct Node* alt) {
    struct Node* n = alt ? alt->next : *pp;
    while (n != 0) {
        int b = (int)(n->inner->id != key);
        if (b == 0) return n;
        n = n->next;
    }
    return 0;
}
