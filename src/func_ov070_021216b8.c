typedef int Fix12;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, Fix12, unsigned int);
extern void func_ov070_02121ae0(void*, void*, int, int);
extern void* data_ov070_021234dc;
extern void* data_ov070_021222e8;
int func_ov070_021216b8(void* c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0xd4, &data_ov070_021234dc, 0x40000000, 0x1000, 0);
    func_ov070_02121ae0((char*)c + 0x38c, &data_ov070_021222e8, 0x47, 0);
    *(int*)((char*)c + 0x3a0) = 1;
    return 1;
}
