//cpp
extern "C" {
struct BCA_File;
extern int data_ov006_021405c8;
extern struct BCA_File* data_ov006_0214057c;
extern double data_ov006_0213b18c;
}
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, struct BCA_File* f, int frame, int speed, unsigned mode);
extern "C" void func_ov006_020ccc8c(char* c)
{
    *(int*)(c + 0x40) = 0;
    *(int*)(c + 0x20) = data_ov006_021405c8;
    *(int*)(c + 0x24) = 0;
    *(int*)(c + 0x34) = 0;
    *(int*)(c + 0x38) = 0;
    *(int*)(c + 0x3c) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, data_ov006_0214057c, 0x40000000, 0x800, 0);
    *(double*)(c + 0x64) = data_ov006_0213b18c;
}
