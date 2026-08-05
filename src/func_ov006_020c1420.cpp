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

struct P2 { int w[2]; };

extern "C" int func_ov006_020c1718(int *r0);
extern P2 data_ov006_0213ac48;

extern "C" void func_ov006_020c1420(char *c, short arg1, int arg2)
{
    *(short *)(c + 0x1de) = arg1;
    *(int *)(c + 0x1d4) = arg2;
    if (func_ov006_020c1718((int *)c) && *(int *)(c + 0x7c) != *(int *)(c + 0x234)) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(BCA_File *)*(void **)(c + 0x204), 0, 0x40000000, 0x800, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(BCA_File *)*(void **)(c + 0x214), 0, 0x40000000, 0x800, 0);
    }
    *(P2 *)c = data_ov006_0213ac48;
}
