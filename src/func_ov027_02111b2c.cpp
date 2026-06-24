//cpp
typedef long long s64;
struct BCA_File;
struct E { char pad[0xc]; };
struct DataPtr { int f[2]; };
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, struct BCA_File* f, int a, int b, unsigned int c);
extern struct E data_ov027_02113a1c[];
extern struct E data_ov027_02113a20[];
extern struct E data_ov027_02113a24[];
extern struct E data_ov027_02113a26[];
extern struct DataPtr data_ov027_02113c8c;
extern struct DataPtr data_ov027_02113c74;

void func_ov027_02111b2c(char* c){
    *(int*)(c + 0x3d4) = *(int*)&data_ov027_02113a1c[*(unsigned char*)(c + 0x3d9)];
    *(int*)(c + 0x98) = *(int*)&data_ov027_02113a20[*(unsigned char*)(c + 0x3d9)];
    *(short*)(c + 0x94) = *(short*)&data_ov027_02113a24[*(unsigned char*)(c + 0x3d9)];
    if (*(short*)&data_ov027_02113a24[*(unsigned char*)(c + 0x3d9)] != *(short*)&data_ov027_02113a26[*(unsigned char*)(c + 0x3d9)]) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, (struct BCA_File*)data_ov027_02113c8c.f[1], 0, 0x1000, 0);
        *(int*)(c + 0x37c) = 0x1000;
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, (struct BCA_File*)data_ov027_02113c74.f[1], 0, 0x1000, 0);
        *(int*)(c + 0x37c) = (int)(((s64)*(int*)&data_ov027_02113a20[*(unsigned char*)(c + 0x3d9)] * 0x5000 + 0x800) >> 12);
    }
    *(int*)(c + 0x3d0) = 1;
}
}
