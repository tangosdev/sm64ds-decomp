struct G6134 { char pad[0x14]; int arr[0x10]; };
extern struct G6134 data_020a6134;

int func_02058538(void) {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (data_020a6134.arr[i] == 0) {
            return i;
        }
    }
    return -1;
}
