//cpp
struct BCA_File;
struct BlendModelAnim {
    int SetAnim(BCA_File& f, int a, int b, int d, unsigned short e);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File& f, int a, int b, int d, unsigned short e);

extern BCA_File* data_ov062_0211e104[];
extern "C" int func_ov062_0211b880(unsigned char* c) {
    *(int*)(c + 0x390) = 0x2000;
    *(int*)(c + 0xa4) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = 0;
    *(int*)(c + 0x9c) = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim*)(c + 0x334), *data_ov062_0211e104[1], 4, 0, 0x1000, 0);
    return 1;
}
