void func_ov006_0211470c(int *out, int *b);

int func_ov006_0211248c(int *a, int *b) {
    int local[2];
    char *base = (char*)a[1];
    if (a[0xb] != *(int*)(base + 0x4664)) return 0;
    if (b[0] >= 0xdc000) {
        func_ov006_0211470c(local, *(int**)(base + 0x4684));
        if (b[1] >= local[1] - 0x20000) return 1;
    }
    return 0;
}
