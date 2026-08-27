//cpp
struct BCA_File;
struct BlendModelAnim {
    void SetAnim(BCA_File &a, int b, int c, int d, unsigned short e);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File &a, int b, int c, int d, unsigned short e);

extern "C" signed char data_0209f2f8;
extern "C" int data_0209e650;
extern "C" void *data_ov062_0211e114;
extern "C" int RandomIntInternal(int* seed);
extern "C" void func_02012694(int a0, void *a1);

extern "C" int func_ov062_0211bc54(char *thiz)
{
    if (data_0209f2f8 == 0x10) {
        *(unsigned short*)(thiz + 0x100) = 0x32;
    } else {
        *(unsigned short*)(thiz + 0x100) =
            (((unsigned)RandomIntInternal(&data_0209e650) >> 8) & 0xf) + 0x14;
    }
    *(int*)(thiz + 0xa4) = 0;
    *(int*)(thiz + 0xa8) = 0;
    *(int*)(thiz + 0xac) = 0;
    func_02012694(0xee, thiz + 0x74);
    *(int*)(thiz + 0x43c) =
        ((((unsigned)RandomIntInternal(&data_0209e650) >> 8) & 3) << 8) + 0x300;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim*)(thiz + 0x334), *(BCA_File*)((void**)&data_ov062_0211e114)[1], 4, 0, 0x1000, 0);
    return 1;
}
