void func_02046208(void *a, void *b, int i);

void func_020461b4(void *a, void *b) {
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++) {
        func_02046208(a, b, i);
    }
}
