//cpp
typedef int Fix12;
struct BCA_File;
struct BTP_File;
struct ModelAnim { void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c); };
struct Animation { void SetFlags(int f); };
struct TextureSequence { void SetFile(BTP_File &f, int a, Fix12 b, unsigned int c); };

extern void *data_ov077_02127b40[];
extern void *data_ov077_02127b30[];

extern "C" void func_ov077_021244d4(char *c)
{
    ((ModelAnim *)(c + 0xd4))->SetAnim((BCA_File *)data_ov077_02127b40[1], 0, 0x1000, 0);
    ((Animation *)(c + 0x124))->SetFlags(0x40000000);
    *(int *)(c + 0x130) = 0x1000;
    *(int *)(c + 0x12c) = 0;
    ((TextureSequence *)(c + 0x1b0))->SetFile(*(BTP_File *)data_ov077_02127b30[1], 0, 0x1000, 0);
    ((Animation *)(c + 0x1b0))->SetFlags(0x40000000);
    *(int *)(c + 0x1bc) = 0x1000;
    *(int *)(c + 0x1b8) = 0;
    *(int *)(c + 0x3f4) = 1;
}
