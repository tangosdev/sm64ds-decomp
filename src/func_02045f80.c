void func_02045fd4(void *a, void *b, int i);

void func_02045f80(void *a, void *b) {
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++) {
        func_02045fd4(a, b, i);
    }
}
