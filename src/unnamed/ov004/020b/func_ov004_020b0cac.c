struct Elem { char pad[0x134]; };
extern struct Elem data_ov004_020bf648[];
extern void func_ov004_020b3278(struct Elem *e, int c, int a1, int a2, int a3, int s1, short s2);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6) {
    int i;
    for (i = 0; i < 3; i++) {
        if (*(int*)((char*)&data_ov004_020bf648[i] + 0x20) == 0x1d) {
            func_ov004_020b3278(&data_ov004_020bf648[i], c, a1, a2, a3, arg5, arg6);
            return;
        }
    }
}
