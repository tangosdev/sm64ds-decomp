//cpp
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int a, int b, unsigned int u);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* tt, void* file, int a, int b, unsigned int u);
}
extern char data_ov074_02122f34[];
extern char data_ov074_02122f3c[];
extern char data_ov074_02122f38[];

extern "C" void func_ov074_02121a4c(char* c, int idx)
{
    int off = idx * 0xc;
    void* file1 = *(void**)(*(char**)(data_ov074_02122f34 + off) + 4);
    int arg2 = *(int*)(data_ov074_02122f3c + off);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x210, file1, arg2, 0x1000, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        c + 0x3f8, *(void**)(data_ov074_02122f38 + off), 0, 0x1000, 0);
}
