extern int func_0201267c(int a, void *b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
struct S { int w[2]; };
extern struct S data_ov084_02130e24;
void func_ov084_0212ebb4(void *c)
{
    func_0201267c(0xc1, (char*)c + 0x74);
    *(int*)((char*)c + 0x458) = 5;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x110, (void*)data_ov084_02130e24.w[1], 0x40000000, 0x1000, 0);
}
