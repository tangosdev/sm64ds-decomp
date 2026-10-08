extern int *data_ov007_02104bbc;
extern void func_0204fa2c(int *p, int arg);
void func_ov007_020bd9e4(int arg) {
    int i;
    for (i = 0; i < 0x39; i++) {
        if (data_ov007_02104bbc[i] != 0) {
            func_0204fa2c(&data_ov007_02104bbc[i], arg);
        }
    }
}
