extern void func_ov004_020ad940(char *c);
extern int data_ov004_020beb68;

void func_ov004_020ad90c(void) {
    char *p = *(char **)&data_ov004_020beb68;
    if (p == 0) return;
    func_ov004_020ad940(p);
}
