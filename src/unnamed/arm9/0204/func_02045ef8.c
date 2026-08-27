void func_02045f4c(void *a, void *b, int i);

void func_02045ef8(void *a, void *b) {
    int i;
    int count = (*(int **)a)[9];
    for (i = 0; i < count; i++) {
        func_02045f4c(a, b, i);
    }
}
