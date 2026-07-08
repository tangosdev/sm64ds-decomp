//cpp
struct BCA_File;
struct BlendModelAnim {
    void SetAnim(BCA_File &a, int b, int c, int d, unsigned short e);
};
extern "C" signed char LEVEL_ID;
extern "C" int RNG_STATE;
extern "C" void *data_ov062_0211e114;
extern "C" int RandomIntInternal(int* seed);
extern "C" void func_02012694(int a0, void *a1);

extern "C" int func_ov062_0211bc54(char *thiz)
{
    if (LEVEL_ID == 0x10) {
        *(unsigned short*)(thiz + 0x100) = 0x32;
    } else {
        *(unsigned short*)(thiz + 0x100) =
            (((unsigned)RandomIntInternal(&RNG_STATE) >> 8) & 0xf) + 0x14;
    }
    *(int*)(thiz + 0xa4) = 0;
    *(int*)(thiz + 0xa8) = 0;
    *(int*)(thiz + 0xac) = 0;
    func_02012694(0xee, thiz + 0x74);
    *(int*)(thiz + 0x43c) =
        ((((unsigned)RandomIntInternal(&RNG_STATE) >> 8) & 3) << 8) + 0x300;
    ((BlendModelAnim*)(thiz + 0x334))->SetAnim(
        *(BCA_File*)((void**)&data_ov062_0211e114)[1], 4, 0, 0x1000, 0);
    return 1;
}
