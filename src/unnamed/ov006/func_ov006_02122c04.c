extern void func_ov006_02122a1c(void* self);
extern int data_ov006_02142f7c;
extern int data_ov006_02142f74;
void func_ov006_02122c04(int x, int count) {
    int i;
    data_ov006_02142f7c = x;
    data_ov006_02142f74 = count;
    for (i = 0; i < data_ov006_02142f74; i++) {
        func_ov006_02122a1c((void*)(data_ov006_02142f7c + i * 0x78));
    }
}
