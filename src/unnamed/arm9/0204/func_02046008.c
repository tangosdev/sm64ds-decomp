void func_0204605c(void *a, void *b, int i);

void func_02046008(void *a, void *b) {
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++) {
        func_0204605c(a, b, i);
    }
}
