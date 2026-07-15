//cpp
typedef short s16;
typedef int Fix12;
struct ModelAnim { void SetAnim(void* bca, int b, Fix12 c, unsigned int d); };
extern "C" {
void func_ov006_020e6df0(int a0, char* a1, void* a2);
void func_ov006_020cb2b4(char* c);
extern int data_ov006_021405c8[];
extern void *data_ov006_0214054c;
extern int data_ov006_0213b15c[];
extern int data_ov006_0214055c[];
}
extern "C" void func_ov006_020cb528(char* c)
{
    int a, b;
    *(int*)(c+0x20) = data_ov006_021405c8[*(s16*)(c+0x52)];
    *(int*)(c+0x34) = 0;
    *(int*)(c+0x38) = 0;
    func_ov006_020e6df0(0, (char*)0x10, *(void**)(c+0x1c));
    ((ModelAnim*)(c+0x6c))->SetAnim(data_ov006_0214054c, 0x40000000, 0x800, 0);
    data_ov006_0214055c[0] = data_ov006_0214055c[0] + 1;
    a = data_ov006_0213b15c[0];
    b = data_ov006_0213b15c[1];
    *(int*)(c+0x64) = b ? a : a;
    *(int*)(c+0x68) = b;
    func_ov006_020cb2b4(c);
}
