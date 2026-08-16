extern void func_ov004_020b67f8(void);
extern void func_ov004_020b0a54(int c);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c0c80(void *c);
extern void func_ov006_020c0d68(char *c);
extern char *data_ov004_020beb68;

void func_ov006_020f4888(char *self)
{
    if (*(unsigned short *)(self + 0x5324) != 0) {
        *(unsigned short *)(((int)self + 0x5324)) -= 1;
        if (*(short *)(self + 0x5324) <= 0) {
            *(unsigned short *)(self + 0x5324) = 0;
            if (*(unsigned char *)(self + 0x5337) >= *(unsigned short *)(self + 0x532a)) {
                if (*(unsigned char *)(self + 0x533a) == 0)
                    func_ov004_020b67f8();
                func_ov004_020b0a54(4);
                if (*(int *)(self + 0xb4) < 0x270f) {
                    int *c = (int *)(((int)self + 0xb4));
                    *c += 1;
                }
                if (*(int *)(self + 0xb4) > *(int *)(self + 0xb8))
                    *(int *)(self + 0xb8) = *(int *)(self + 0xb4);
                func_ov004_020adb1c(data_ov004_020beb68 ? *(int *)(data_ov004_020beb68 + 0xb4) : 0);
                func_ov006_020c0c80(self + 0x4f38);
            } else {
                func_ov004_020b0a54(5);
                if (*(int *)(self + 0xb4) > 0) {
                    int *c = (int *)(((int)self + 0xb4));
                    *c -= 1;
                }
                func_ov006_020c0d68(self + 0x4f38);
            }
        }
    } else {
        *(unsigned char *)(self + 0xc3) = 0;
        *(unsigned short *)(self + 0x5322) = 0x10;
    }
}
