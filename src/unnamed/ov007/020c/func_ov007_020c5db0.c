struct E {
    char pad8[8];
    unsigned short v;
};
struct C {
    char pad8[8];
    int count;
    char pad_to20[0x14];
    struct E *first;
    char pad_to38[0x14];
    struct E **arr;
};
int func_ov007_020c5db0(struct C *c)
{
    int i;
    int total;
    int count;
    total = c->first->v;
    count = c->count;
    i = 0;
    if (count > 0) {
        do {
            struct E *e = c->arr[i];
            i++;
            total += e->v;
        } while (i < count);
    }
    return total;
}
