typedef struct { int x; int pad[4]; } E;
typedef struct { int a; char b; E e[64]; int arr[16]; } S;
void func_02011a5c(S *c) {
    int i;
    E *p = c->e;
    do { p->x = 0; p++; } while (p != c->e + 64);
    c->a = 0;
    c->b = 0;
    for (i = 0; i < 16; i++) c->arr[i] = 0;
}
