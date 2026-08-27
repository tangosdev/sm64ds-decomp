struct Node {
    char pad[4];
    void *cached;
};
extern void *func_0203b358(struct Node *thiz);
void *func_0203b394(struct Node *thiz) {
    void *p = thiz->cached;
    if (p == 0)
        p = func_0203b358(thiz);
    return p;
}
