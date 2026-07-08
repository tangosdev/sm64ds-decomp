//cpp
extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
void LoadBlueCoinModel(void* actor);
int _ZN11ShadowModel12InitCylinderEv(void* self);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int r, int h, unsigned int d, unsigned int e);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int b, int c, void* v, int e);
void* _ZN5Actor10FindWithIDEj(unsigned int id);
void func_ov096_02136928(void* self, int a);
void func_ov096_02135efc(void* self);
}

extern void* data_ov096_02137b20;
extern void* data_ov096_02137b28;

struct Block48 { int w[12]; };
struct Block3 { int w[3]; };
extern Block48 _ZN9Matrix3x38IDENTITYE;

extern "C" int func_ov096_02136ab0(char* c)
{
    int t;

    t = (*(unsigned short*)(c + 0xc) == 0xf0);
    if (t != false) {
        void* m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137b20);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, 1);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137b28);
        LoadBlueCoinModel(c);
        *(unsigned char*)(c + 0x3a8) = 1;
    } else {
        t = (*(unsigned short*)(c + 0xc) == 0xf1);
        if (t != false) {
            void* m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137b28);
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, 1) == 0)
                return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x124) == 0)
        return 0;

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x14c, c, 0x3c000, 0x78000, 0x200004, 0x6eff0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x180, c, 0x3c000, 0x3c000, 0, 0);

    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x98) = 0x3000;

    t = (*(unsigned short*)(c + 0xc) == 0xf0);
    if (t != false) {
        *(int*)(c + 0x36c) = *(int*)(c + 0x5c);
        *(int*)(c + 0x370) = *(int*)(c + 0x60);
        *(int*)(c + 0x374) = *(int*)(c + 0x64);
        *(int*)(c + 0x80) = 0x1000;
        *(int*)(c + 0x84) = 0x1000;
        *(int*)(c + 0x88) = 0x1000;
        *(int*)(c + 0x390) = 0;
        *(int*)(c + 0x394) = 0;
    } else {
        t = (*(unsigned short*)(c + 0xc) == 0xf1);
        if (t != false) {
            *(int*)(c + 0x80) = 0;
            *(int*)(c + 0x84) = 0;
            *(int*)(c + 0x88) = 0;
            *(void**)(c + 0x390) = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 8));
            *(int*)(c + 0x394) = 0;
            {
                int* p = (int*)(((int)*(char**)(c + 0x390) + 0x36c) & 0xFFFFFFFFFFFFFFFF);
                *(int*)(c + 0x36c) = p[0];
                *(int*)(c + 0x370) = p[1];
                *(int*)(c + 0x374) = p[2];
            }
        }
    }

    func_ov096_02136928(c, 1);
    *(Block48*)(c + 0x33c) = _ZN9Matrix3x38IDENTITYE;
    func_ov096_02135efc(c);
    return 1;
}
