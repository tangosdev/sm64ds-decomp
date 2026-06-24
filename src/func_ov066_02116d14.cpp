//cpp
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
    void SetAnim(BCA_File &f, int a, int b, Fix12 c, unsigned short d);
};

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
        ((BlendModelAnim *)(c + 0x360))->SetAnim(*(BCA_File *)data_ov066_0211aea4[1], 4, 0x40000000, 0x1000, 0);
    } else {
        ((BlendModelAnim *)(c + 0x360))->SetAnim(*(BCA_File *)data_ov066_0211ae8c[1], 4, 0x40000000, 0x1000, 0);
    }
    return 1;
}
