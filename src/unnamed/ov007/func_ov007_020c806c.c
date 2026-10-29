extern void *func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c806c(void *c);
extern int func_ov007_020c3d1c(void);
extern void MultiStore_Int(int val, void *dst, int size);

void *func_ov007_020c80a4(void) {
    void *c = func_ov007_020c3df4(0, 0x18);
    func_ov007_020c806c(c);
    return c;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
int func_ov007_020c8098(void) {
    return func_ov007_020c3d1c();
}
#pragma long_calls off

void func_ov007_020c806c(void *c) {
    volatile int li = 0;
    MultiStore_Int(li, c, 0x18);
}
