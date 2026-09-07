typedef struct UID { int low, high; } UID;

typedef struct dActor_c {
    int pad[0x30 / 4];
    UID id;                             /* 0x30 */
    unsigned char pad2[0xe6 - 0x38];
    short fE6;                          /* 0xe6 */
} dActor_c;

extern UID data_ov006_0213afa0;
extern UID data_ov006_0213afb0;

int func_ov006_020c4710(dActor_c *a)
{
    int result = 0;
    UID *p = (UID *)(int)(&a->id);
    UID *c = (UID *)(int)(&data_ov006_0213afa0);
    int neq = 1;
    if (p->low == c->low) {
        if (p->high == c->high || a->id.low == 0)
            neq = 0;
    }
    if (neq != 0) {
        int flag = 1;
        int neq2 = flag;
        UID *q = (UID *)(int)((long long)(unsigned int)&a->id);
        UID *c2 = (UID *)(int)(&data_ov006_0213afb0);
        if (q->low == c2->low) {
            if (q->high == c2->high || a->id.low == 0)
                neq2 = 0;
        }
        if (neq2 == 0) {
            if (a->fE6 != 0)
                flag = 0;
        }
        if (flag != 0)
            result = 1;
    }
    return result;
}
