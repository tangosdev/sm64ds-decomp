extern int func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern int data_ov006_0212f0c8[];
extern int data_ov006_02134f24;

void func_ov006_0212093c(short* obj, int arg1) {
    if (*(int*)((char*)obj + 4) == 0) return;
    if (*(int*)((char*)obj + 8) == 0) return;
    func_ov004_020afdd0(
        data_ov006_02134f24,
        0xd0,
        data_ov006_0212f0c8[obj[0]] - arg1 - 8,
        -1,
        -1);
}
