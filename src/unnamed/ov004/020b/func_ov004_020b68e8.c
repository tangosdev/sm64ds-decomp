extern void *data_ov004_020beb68;
extern void func_ov004_020b14f0(void *);
extern void func_ov004_020b0d8c(void *, int, int);

void func_ov004_020b68e8(int *c) {
    void *g = data_ov004_020beb68;
    if (g == 0) return;
    func_ov004_020b14f0(g);
    if (c[7] > 0) return;
    func_ov004_020b0d8c(g, 0xe0, 0xa0);
}
