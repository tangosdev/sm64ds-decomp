extern int SetAnim(char* anim, void* file, int a, int b, unsigned int u);
struct S { int w[2]; };
extern struct S data_ov070_02123520;
int func_ov070_0211fd60(char *p) {
    SetAnim(p+0x300, ((void**)&data_ov070_02123520)[1], 0, 0x1000, 0);
    return 1;
}
