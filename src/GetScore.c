extern int data_ov004_020beb68;

int GetScore(void) {
    int val = data_ov004_020beb68;
    if (val != 0) {
        return *(int*)(val + 0x464c);
    }
    return 0;
}
