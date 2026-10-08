typedef struct A {
    int f0;
    int f4;
    int f8;
    struct A* fc;
} A;

int func_02035354(A* a, A* b){
    if (b == 0 || a->fc == 0) return 0;
    {
        int t = b->f4;
        if (a->fc == b && a->f8 == t) return 1;
    }
    return 0;
}
