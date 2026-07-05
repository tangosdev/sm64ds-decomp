//cpp
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int a, int b, unsigned int u);
void func_ov102_0214b384(void* c, int i);
}
struct G2 { int w[2]; };
extern G2 data_ov102_0214e9c8;

extern "C" void func_ov102_0214bd20(char* c)
{
    *(int*)(c + 0x3dc) = 3;
    *(int*)(c + 0x9c) = 0;
    *(int*)(c + 0x98) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)data_ov102_0214e9c8.w[1], 0, 0x1000, 0);
    func_ov102_0214b384(c, 0x96);
    {
        int* p = (int*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFF);
        *p = *p | 2;
        *p = *p & ~4;
    }
}
