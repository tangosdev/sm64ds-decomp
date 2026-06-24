//cpp
typedef int Fix12i;
struct BCA_File; struct BTP_File; struct BTA_File;
struct ModelAnim { int d; };
struct TextureSequence { int d; };
struct TextureTransformer { int d; };

extern "C" unsigned int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim*, BCA_File*, int, Fix12i, unsigned int);
extern "C" unsigned int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    TextureSequence*, BTP_File&, int, Fix12i, unsigned int);
extern "C" unsigned int _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer*, BTA_File&, int, Fix12i, unsigned int);

struct D1 { int a; BCA_File* f; };
struct D2 { int a; BTP_File* f; };
extern D1 data_ov070_021235f4;
extern D2 data_ov070_021235ec;
extern BTA_File data_ov070_021231f4;

extern "C" void func_ov070_02120bf8(char* thiz)
{
    char* c = thiz;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (ModelAnim*)(c + 0xd4), data_ov070_021235f4.f, 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        (TextureSequence*)(c + 0x188), *data_ov070_021235ec.f, 0, 0x1000, 0);
    *(int*)(c + 0x194) = 0x1000;
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        (TextureTransformer*)(c + 0x19c), data_ov070_021231f4, 0, 0x1000, 0);
    *(int*)(c + 0x1a8) = 0x2000;
    *(unsigned char*)(c + 0x430) = 0xf;
    *(int*)(c + 0x424) = 0;
    *(short*)(c + 0x42e) = 0;
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    *(int*)(c + 0x420) = 1;
}
