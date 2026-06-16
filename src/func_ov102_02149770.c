
extern int func_ov102_02149078(void *c);
extern void func_ov102_02149da8(void *c, int i);
typedef unsigned char u8;
void func_ov102_02149770(void *c, void *x) {
    if (*(int *)((char *)c + 0x3e8) == 1) return;
    int r = func_ov102_02149078(c);
    if (r != 0) return;
    *(u8 *)((char *)c + 0x3f2) = *(int *)((char *)x + 8);
    func_ov102_02149da8(c, 1);
}
