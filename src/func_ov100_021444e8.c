extern int func_ov100_02144fcc(void *c, void *p);
extern void func_ov100_021453d8(void *c, void *p, void *a2);
extern char data_ov100_021488b4[];
int func_ov100_021444e8(void *c, void *a1) {
    if (func_ov100_02144fcc(c, a1)) {
        func_ov100_021453d8(c, data_ov100_021488b4, a1);
    }
    return 1;
}
