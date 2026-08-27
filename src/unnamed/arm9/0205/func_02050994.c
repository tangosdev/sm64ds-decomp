extern int *data_020a5bb8;
void func_02050994(int i, int val) {
    int *p = (int*)data_020a5bb8[0x7c / 4];
    (p + i * 4)[5] = val;
}
