typedef int Fix12;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, Fix12, unsigned int);
extern void func_ov070_02121ae0(void*, void*, int, int);
extern void* data_ov070_021234c4;
extern void* data_ov070_02122404;
int func_ov070_021217ac(char* c){
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, &data_ov070_021234c4, 0, 0x1000, 0);
    func_ov070_02121ae0(c + 0x38c, &data_ov070_02122404, 0x64, 1);
    *(unsigned char*)(c + 0x3ac) = 0x73;
    *(int*)(c + 0x3a0) = 0;
    return 1;
}
