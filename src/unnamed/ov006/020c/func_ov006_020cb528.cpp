//cpp
typedef short s16;
typedef int Fix12;
struct ModelAnim { void SetAnim(void* bca, int b, Fix12 c, unsigned int d); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void* bca, int b, Fix12 c, unsigned int d);

extern "C" {
void Sound_PlayBank1Panned(int a0, char* a1, void* a2);
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
    Sound_PlayBank1Panned(0, (char*)0x10, *(void**)(c+0x1c));
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim*)(c+0x6c), data_ov006_0214054c, 0x40000000, 0x800, 0);
    data_ov006_0214055c[0] = data_ov006_0214055c[0] + 1;
    a = data_ov006_0213b15c[0];
    b = data_ov006_0213b15c[1];
    *(int*)(c+0x64) = b ? a : a;
    *(int*)(c+0x68) = b;
    func_ov006_020cb2b4(c);
}
