extern char data_ov070_021236ac;
extern void func_ov070_02121848(char *c);
void func_ov070_02121880(char *c, int a) {
    *(int *)(c + 0x39c) = (int)&data_ov070_021236ac + (a << 4);
    func_ov070_02121848(c);
}
