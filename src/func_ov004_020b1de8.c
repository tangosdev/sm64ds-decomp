extern int GetGameLanguage(void);
extern int *data_ov004_020bbfa8[];
extern int func_ov004_020b1d60(int, int, int, int, int);

void func_ov004_020b1de8(int r0, int r1, int r2, int r3) {
    int idx = GetGameLanguage();
    int *t = data_ov004_020bbfa8[idx];
    func_ov004_020b1d60(t[2], r0, r1, r2, r3);
}
