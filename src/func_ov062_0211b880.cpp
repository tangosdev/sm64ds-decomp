//cpp
struct BCA_File;
struct BlendModelAnim {
    int SetAnim(BCA_File& f, int a, int b, int d, unsigned short e);
};
extern BCA_File* data_ov062_0211e104[];
extern "C" int func_ov062_0211b880(unsigned char* c) {
    *(int*)(c + 0x390) = 0x2000;
    *(int*)(c + 0xa4) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = 0;
    *(int*)(c + 0x9c) = 0;
    ((BlendModelAnim*)(c + 0x334))->SetAnim(*data_ov062_0211e104[1], 4, 0, 0x1000, 0);
    return 1;
}
