extern unsigned char data_ov066_0211ae0c[];
extern unsigned char data_ov066_0211abe0[];
extern int func_ov066_021168ec(void *c);

int func_ov066_02118604(void *c) {
    int r = func_ov066_021168ec(c);
    if (r != 0) {
        *data_ov066_0211ae0c ^= *(int*)((char*)c + 0x49c);
        if (*data_ov066_0211abe0 != 3) {
            *data_ov066_0211ae0c |= 3;
        }
    }
    return 1;
}
