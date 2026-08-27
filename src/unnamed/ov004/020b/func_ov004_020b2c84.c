extern void *data_ov004_020bf560[];
extern void Deallocate(void *ptr);
void func_ov004_020b2c84(void) {
    int i;
    for (i = 0; i < 0x1d; i++) {
        Deallocate(data_ov004_020bf560[i]);
        data_ov004_020bf560[i] = 0;
    }
}
