extern int func_ov006_020f9cbc(void *a, void *b);

struct Node { char pad[8]; struct Node *next; char pad2[0x1e]; short key; };
extern struct Node *data_ov006_0214257c;

int func_ov006_020f96e0(void)
{
    struct Node *p, *q;
    p = data_ov006_0214257c;
    while (p != 0 && p->key < 0x14) {
        q = p->next;
        while (q != 0 && q->key < 0x14) {
            if (func_ov006_020f9cbc(p, q) != 0) return 1;
            q = q->next;
        }
        p = p->next;
    }
    return 0;
}
