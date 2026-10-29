extern void *func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c1048(int *c);
extern int func_ov007_020c3d1c(void);

void *func_ov007_020c1068(void) {
    void *c = func_ov007_020c3df4(0, 0xc);
    func_ov007_020c1048(c);
    return c;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
int func_ov007_020c105c(void) {
    return func_ov007_020c3d1c();
}
#pragma long_calls off

void func_ov007_020c1048(int *c) {
    c[1] = 0;
    c[0] = c[1];
}
