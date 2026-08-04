//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int data_ov006_021405c8[];
extern void *data_ov006_02140564;
extern double data_ov006_0213b1e4;
void func_ov006_020cc698(char *c);
void func_ov006_020cc698(char *c) {
    *(int*)(c + 0x40) = 0;
    *(int*)(c + 0x20) = data_ov006_021405c8[*(short*)(c + 0x52)];
    *(int*)(c + 0x24) = 0;
    *(int*)(c + 0x34) = -0x1800;
    *(int*)(c + 0x38) = 0;
    *(int*)(c + 0x3c) = 0;
    *(short*)(c + 0x4a) = -0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, *(void**)&data_ov006_02140564, 0, 0x800, 0);
    *(double*)(c + 0x64) = data_ov006_0213b1e4;
}
}
