extern int data_ov004_020beb68;

void func_ov004_020adc00(int v) {
    int val = data_ov004_020beb68;
    if (val != 0) {
        *(int*)(val + 0x4650) = v;
    }
}
