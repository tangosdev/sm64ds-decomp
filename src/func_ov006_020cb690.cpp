//cpp
struct ModelAnim { void SetAnim(void* bca, int b, int c, unsigned int d); };
extern "C" {
void func_ov006_020e6df0(int a0, char* a1, void* a2);
void func_ov006_020cb5c4(char* c);
extern int _ZN7Vector38IDENTITYE[];
extern int *data_ov006_021405c0;
struct W2 { int a, b; };
extern struct W2 data_ov006_0213b224;
void func_ov006_020cb690(char* c);
}
void func_ov006_020cb690(char* c)
{
    *(int*)(c+0x34) = _ZN7Vector38IDENTITYE[0];
    *(int*)(c+0x38) = _ZN7Vector38IDENTITYE[1];
    *(int*)(c+0x3c) = _ZN7Vector38IDENTITYE[2];
    ((ModelAnim*)(c+0x6c))->SetAnim((void*)data_ov006_021405c0, 0x40000000, 0x800, 0);
    func_ov006_020e6df0(0, (char*)0x17, *(void**)(c+0x1c));
    {
        int a = data_ov006_0213b224.a;
        int b = data_ov006_0213b224.b;
        *(int*)(c+0x64) = b ? a : a;
        *(int*)(c+0x68) = b;
    }
    int* p20 = (int*)(((int)c + 0x20) & 0xFFFFFFFFFFFFFFFF);
    *p20 = *p20 + 0x20000;
    func_ov006_020cb5c4(c);
}
