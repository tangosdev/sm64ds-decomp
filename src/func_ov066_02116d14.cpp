//cpp
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
    void SetAnim(BCA_File &f, int a, int b, Fix12 c, unsigned short d);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File &f, int a, int b, Fix12 c, unsigned short d);


extern void *data_ov066_0211aea4[];
extern void *data_ov066_0211ae8c[];

extern "C" int func_ov066_02116d14(char *c)
{
    *(int *)(c + 0x494) = 0;
    *(int *)(c + 0x498) = 0;
    *(short *)(c + 0x4d0) = 0;
    *(int *)(c + 0x4a0) = 0;
    *(int *)(c + 0x98) = -0xa000;
    if (*(int *)(c + 0x49c) == 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x360), *(BCA_File *)data_ov066_0211aea4[1], 4, 0x40000000, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x360), *(BCA_File *)data_ov066_0211ae8c[1], 4, 0x40000000, 0x1000, 0);
    }
    return 1;
}
