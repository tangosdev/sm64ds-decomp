//cpp
struct SharedFilePtr;
struct BMD_File; struct BCA_File;
struct Model { static BMD_File *LoadFile(SharedFilePtr &f); };
struct Animation { static BCA_File *LoadFile(SharedFilePtr &f); };
struct ModelBase { int SetFile(BMD_File *f, int a, int b); };
struct ModelAnim { int SetAnim(BCA_File *f, int a, int b, unsigned int u); };

extern "C" void func_02016acc(char *m, int a);
extern "C" void func_02016b24(char *m, int a);
extern "C" void func_02016a14(char *m, int a);
extern "C" void func_02016a04(char *m, int a);
extern "C" void func_ov006_020c4c00(char *c);
extern "C" void func_ov006_020c4d20(char *p);

extern SharedFilePtr data_ov006_02140330;
extern SharedFilePtr data_ov006_02140338;

struct GObj { int w[8]; };
extern GObj *data_0209f5c0;

extern "C" int func_ov006_020c6e4c(char *c) {
    BMD_File *m = Model::LoadFile(data_ov006_02140330);
    BCA_File *a = Animation::LoadFile(data_ov006_02140338);
    if (((ModelBase *)(c + 0x38))->SetFile(m, 1, -1) == 0) {
        return 0;
    }
    ((ModelAnim *)(c + 0x38))->SetAnim(a, 0, 0x800, 0);
    func_02016acc(c + 0x38, 1);
    func_02016b24(c + 0x38, 2);
    int b = (*(unsigned short *)((char *)data_0209f5c0 + 0xc) == 0x175);
    if (b != 0) {
        func_02016a14(c + 0x38, 0x7fff);
        func_02016a04(c + 0x38, 0x7d40);
    }
    func_ov006_020c4c00(c);
    func_ov006_020c4d20(c);
    return 1;
}
