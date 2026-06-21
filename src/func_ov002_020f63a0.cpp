//cpp
extern "C" {
extern void func_ov002_020f5f60(void *self);
extern void _ZN6Memory16operator_delete2EPv(void *p);
int func_ov002_020f63a0(char *r0) {
    char *r4 = r0;
    func_ov002_020f5f60(*(void**)(r4 + 0xd8));
    if (*(void**)(r4 + 0xd8) != 0) {
        _ZN6Memory16operator_delete2EPv(*(void**)(r4 + 0xd8));
        *(void**)(r4 + 0xd8) = 0;
    }
    return 1;
}
}
