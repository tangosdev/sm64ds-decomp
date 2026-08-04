extern void func_ov006_020eed68(void *p);
extern void *data_ov006_021421c0;

void func_ov006_020eef58(void) {
    void *node = data_ov006_021421c0;
    if (node == 0) return;
    do {
        func_ov006_020eed68(node);
        node = *(void **)node;
    } while (node != 0);
}
