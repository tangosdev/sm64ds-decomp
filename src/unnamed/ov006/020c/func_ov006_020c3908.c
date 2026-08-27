extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* anim, void* file, int a, int b, unsigned int u);
struct S { int w[2]; };
extern struct S data_ov006_0213aed8;
void func_ov006_020c3908(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0xd18, *(void**)(p+0xda0), 0x40000000, 0x800, 0);
    *(struct S *)(p + 0x0) = data_ov006_0213aed8;
}
