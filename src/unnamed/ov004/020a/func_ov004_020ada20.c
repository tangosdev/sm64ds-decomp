extern int data_ov004_020beb68;

void func_ov004_020ada20(void) {
    char *ptr = *(char **)&data_ov004_020beb68;
    if (ptr != 0) {
        *(ptr + 0x4000 + 0x65c) = 0;
    }
}
