extern int SetAnim(char* anim, void* file, int a, int b, unsigned int u);
extern void** data_ov034_02113888[2];
void func_ov034_02111720(char *p) {
    *(char*)(p+0x8da)=0;
    SetAnim(p+0x110, data_ov034_02113888[0][1], 0x40000000, 0x1000, 0);
    SetAnim(p+0x174, data_ov034_02113888[1][1], 0x40000000, 0x1000, 0);
}
