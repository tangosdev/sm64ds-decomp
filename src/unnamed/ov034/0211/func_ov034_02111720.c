extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* anim, void* file, int a, int b, unsigned int u);
extern void** data_ov034_0211389c[2];
void func_ov034_02111720(char *p) {
    *(char*)(p+0x8da)=0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x110, data_ov034_0211389c[0][1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x174, data_ov034_0211389c[1][1], 0x40000000, 0x1000, 0);
}
