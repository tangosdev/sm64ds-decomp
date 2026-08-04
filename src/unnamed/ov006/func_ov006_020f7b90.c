extern int func_ov006_020f8224(void *a, void *b);

struct Node { char pad[8]; struct Node *next; char pad2[0x1e]; short key; };
extern struct Node *data_ov006_02142500;

int func_ov006_020f7b90(void)
{
    struct Node *p, *q;
    p = data_ov006_02142500;
    while (p != 0 && p->key < 0x14) {
        q = p->next;
        while (q != 0 && q->key < 0x14) {
            if (func_ov006_020f8224(p, q) != 0) return 1;
            q = q->next;
        }
        p = p->next;
    }
    return 0;
}
