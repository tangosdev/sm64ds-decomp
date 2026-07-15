struct Pair { int a, b; };
extern struct Pair data_ov006_0213b058;
extern struct Pair data_ov006_0213b070;

int func_ov006_020c762c(char *c)
{
    struct Pair *p;
    struct Pair *g;
    int r = 0;
    int m = 1;

    p = (struct Pair *)(((int)c + 0x3c) & 0xFFFFFFFFFFFFFFFF);
    g = &data_ov006_0213b058;
    if (p->a == g->a) {
        if (p->b == g->b)
            goto cl1;
        if (*(int *)(c + 0x3c) == 0) {
cl1:
            m = 0;
        }
    }
    if (m != 0) {
        m = 1;
        p = (struct Pair *)(((long long)(int)(c + 0x3c)) & 0xFFFFFFFFFFFFFFFFLL);
        g = &data_ov006_0213b070;
        if (p->a == g->a) {
            if (p->b == g->b)
                goto cl2;
            if (*(int *)(c + 0x3c) == 0) {
cl2:
                m = 0;
            }
        }
        if (m != 0)
            r = 1;
    }
    return r;
}
