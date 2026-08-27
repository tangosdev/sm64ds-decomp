extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
struct S { int w[2]; };
extern struct S data_ov062_0211e02c;
void func_ov062_02119954(void *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x300, (void*)data_ov062_0211e02c.w[1], 0x40000000, 0x1000, 0);
    *(int*)((char*)c + 0xa8) = 0x2d000;
    *(int*)((char*)c + 0x9c) = -0x4000;
    *(unsigned char*)((char*)c + 0x390) = 3;
}
