extern void func_ov007_020c831c(void* a, int b, int c);

void func_ov007_020c7504(int* self, int b, int c) {
    int i;
    for (i = 0; i < self[1]; i++) {
        func_ov007_020c831c(((void**)self[0])[i], b, c);
    }
}
