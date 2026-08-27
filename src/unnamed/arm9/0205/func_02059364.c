struct Node89 {
    struct Node89 *f0;
    struct Node89 *f4;
};

void *func_02059364(void *a, struct Node89 *n) {
    if (n->f4 != 0) n->f4->f0 = n->f0;
    if (n->f0 == 0) return n->f4;
    n->f0->f4 = n->f4;
    return a;
}
