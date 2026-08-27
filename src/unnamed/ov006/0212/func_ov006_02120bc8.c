extern void *data_ov006_02142f64;
void func_ov006_02120bc8(int *o) {
    int *node;
    o[0] = (int)data_ov006_02142f64;
    data_ov006_02142f64 = o;
    node = (int*)o[0];
    int v = o[2] - 0x10000;
    while (node) {
        node[4] = v;
        node = (int*)node[0];
        v -= 0x10000;
    }
}
