extern int data_ov004_020beb68;

int func_ov004_020adc1c(void) {
    int val = data_ov004_020beb68;
    if (val != 0) {
        return *(int*)(val + 0x4650);
    }
    return 0;
}
