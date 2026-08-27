struct Node65 { int key; char pad[0x24]; struct Node65 *next; };

extern struct Node65 *func_02065ae0(void);

struct Node65 *func_02065a94(int key) {
    struct Node65 *p = func_02065ae0();
    while (p != 0) {
        if (p->key == key) return p;
        p = p->next;
    }
    return 0;
}
