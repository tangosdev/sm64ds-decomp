void func_02046288(void *a, void *b, int i);

void func_02046234(void *a, void *b) {
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++) {
        func_02046288(a, b, i);
    }
}
