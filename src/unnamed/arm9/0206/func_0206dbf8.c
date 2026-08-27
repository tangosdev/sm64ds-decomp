typedef void (*FP0206dbf8)(void);

extern int data_020a9ed4;
extern FP0206dbf8 data_020a9ecc;
void func_02072f3c(void);
int func_0206dac4(int a);

int func_0206dbf8(int a) {
    if (data_020a9ed4 == 0) {
        func_02072f3c();
        if (data_020a9ecc != 0) {
            data_020a9ecc();
            data_020a9ecc = 0;
        }
    }
    return func_0206dac4(a);
}
