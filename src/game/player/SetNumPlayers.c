extern void func_020308d0(void);
extern unsigned char data_0208a0e0;

void SetNumPlayers(int n) {
    data_0208a0e0 = n;
    func_020308d0();
}
