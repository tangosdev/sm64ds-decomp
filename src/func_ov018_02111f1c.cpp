//cpp
typedef int Fix12;
struct BCA_File;
struct BTP_File;
struct ModelAnim { void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c); };
struct TextureSequence { void SetFile(BTP_File &f, int a, Fix12 b, unsigned int c); };

extern "C" void func_ov018_021123d0(char *c, int x);
extern void *data_ov018_02113c08[];
extern void *data_ov018_02113bf8[];

extern "C" int func_ov018_02111f1c(char *c)
{
    if (*(int *)(c + 0x374) == 0 && *(unsigned char *)(c + 0x386) == 0)
        func_ov018_021123d0(c, 0);
    *(int *)(c + 0x98) = 0x5000;
    ((ModelAnim *)(c + 0xd4))->SetAnim((BCA_File *)data_ov018_02113c08[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    ((TextureSequence *)(c + 0x138))->SetFile(*(BTP_File *)data_ov018_02113bf8[1], 0, 0x1000, 0);
    *(int *)(c + 0x37c) = 2;
    return 1;
}
