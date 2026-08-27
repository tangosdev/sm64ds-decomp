extern int data_ov004_020beb68;
extern int data_0209b308[];

int func_ov004_020ae1cc(void) {
    int r0 = data_ov004_020beb68;
    if (r0 != 0) {
        return *(int*)((char*)data_0209b308 + 0x2c);
    }
    return 0;
}
