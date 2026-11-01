struct Head {
    int first;
    int last;
    unsigned short count;
    unsigned short id;
};
struct Link {
    int prev;
    int next;
    unsigned short key;
    unsigned short tag;
};
extern int fn_02380884(int a1);
extern void fn_023808C0(int a1);
int fn_02394B88(struct Head *a1, struct Link *a2) {
    unsigned short *p;
    int tok;
    if (a2->tag != 0xBF1D)
        return 1;
    if (a2->key != a1->id)
        return 2;
    tok = fn_02380884(0x1000000);
    p = &a1->count;
    *p = (unsigned short)(*p - 1);
    if (a1->count == 0) {
        a1->first = -1;
        a1->last = -1;
    } else if (a2 == (struct Link *)a1->first) {
        a1->first = a2->next;
        *(int *)a1->first = -1;
    } else if (a2 == (struct Link *)a1->last) {
        a1->last = a2->prev;
        *(int *)(a1->last + 4) = -1;
    } else {
        *(int *)a2->next = a2->prev;
        *(int *)(a2->prev + 4) = a2->next;
    }
    a2->key = 0;
    fn_023808C0(tok);
    return 0;
}
