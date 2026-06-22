extern unsigned int data_020a4d50;
void func_0205aac8(void *a, int b);

int func_0204f194(unsigned int a) {
    if (a == 0) return 1;
    if (a & data_020a4d50) return 0;
    func_0205aac8((void *)a, 0);
    data_020a4d50 |= a;
    return 1;
}
