extern void *data_020a6484;
extern void *data_020a6494;

void *func_0205adf8(void) {
    void *n = data_020a6484;
    if (n == 0) return 0;
    data_020a6484 = *(void **)n;
    if (data_020a6484 == 0) data_020a6494 = 0;
    return n;
}
