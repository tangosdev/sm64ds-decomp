struct Elem {
    char pad[0x2c];
    int a;
    short b;
};
void func_02045e2c(int obj, int i, int a, short b) {
    struct Elem *e = (struct Elem *)(*(int*)(obj + 8) + i * 0x34);
    e->a = a;
    e->b = b;
}
