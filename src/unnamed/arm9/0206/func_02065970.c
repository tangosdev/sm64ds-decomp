struct Node65 { char pad[0x28]; struct Node65 *next; };

struct Node65 *func_02065ae0(void);

int func_02065970(void) {
    int n = 0;
    struct Node65 *p = func_02065ae0();
    if (p != 0) {
        do {
            p = p->next;
            n++;
        } while (p != 0);
    }
    return n;
}
