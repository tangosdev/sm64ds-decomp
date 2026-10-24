extern int *fn_0238E688(void);
extern int fn_02382AC4(void);
extern void fn_02381C08(int, int, int);
extern int fn_02382AD8(int);

int fn_0238F778(int a1) {
    int *base = fn_0238E688();
    int saved = fn_02382AC4();
    fn_02381C08(*(int *)((char *)base + 0x1608), *(int *)((char *)base + 0x160C), a1);
    return fn_02382AD8(saved);
}
