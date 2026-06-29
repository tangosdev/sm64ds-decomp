extern void func_ov006_020eed68(void *p);
extern void *data_ov006_02142f64;

void func_ov006_02120c08(void) {
    void *node = data_ov006_02142f64;
    if (node == 0) return;
    do {
        func_ov006_020eed68(node);
        node = *(void **)node;
    } while (node != 0);
}
