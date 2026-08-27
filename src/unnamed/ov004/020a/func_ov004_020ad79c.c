extern int data_ov004_020beb68;
extern int data_0209b308[];
extern int data_0209b300[];
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
int func_ov004_020adc3c(void *c);
void func_02013568(int a, int b, int v);

void func_ov004_020ad79c(int r0arg, int r1arg) {
    int v0 = r0arg, v1 = r1arg;
    if (data_ov004_020beb68 == 0) return;
    if (v0 <= 0) {
        v0 = data_0209b308[5];
        v1 = data_0209b308[6];
    } else {
        if (v0 > 0x270f) v0 = 0x270f;
    }
    if (v1 <= 0) {
        v1 = data_0209b308[6];
    } else {
        if (v1 > 0x270f) v1 = 0x270f;
    }
    if (v0 == func_ov004_020ad8b8()) {
        if (v1 == func_ov004_020ad878()) return;
    }
    func_02013568(func_ov004_020adc3c((void*)data_ov004_020beb68), 0, v0);
    func_02013568(func_ov004_020adc3c((void*)data_ov004_020beb68), 1, v1);
    *(unsigned char*)data_0209b300 = 1;
}
