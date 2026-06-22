struct Node5b1a4 {
    char pad[4];
    int a;
    int b;
    int c;
    int d;
    int e;
};
extern struct Node5b1a4 *func_0205b1d8(int kind);
extern void func_0205b1a4(struct Node5b1a4 *node);

void func_0205a86c(int a, int b, int c, int d, int e) {
    struct Node5b1a4 *p = func_0205b1d8(1);
    if (p) {
        p->a = a;
        p->b = b;
        p->c = c;
        p->d = d;
        p->e = e;
        func_0205b1a4(p);
    }
}
