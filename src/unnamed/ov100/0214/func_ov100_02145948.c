extern int func_ov100_02144fcc(void *c, void *p);
extern void func_ov100_02145f68(void *c, void *p, void *a2);
extern char data_ov100_02148974[];
int func_ov100_02145948(void *c, void *a1) {
    if (func_ov100_02144fcc(c, a1)) {
        func_ov100_02145f68(c, data_ov100_02148974, a1);
    }
    return 1;
}
