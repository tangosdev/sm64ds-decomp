//cpp
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct BCA_File;
struct Model { int d; };
struct ModelBase { int d; };
struct Animation { int d; };
struct BlendModelAnim { int d; };
struct ShadowModel { int d; };

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    BlendModelAnim*, BCA_File&, int, int, Fix12i, unsigned short);
extern "C" void _ZN5Model12SetPolygonIDEi(Model*, int);
extern "C" void _ZN11ShadowModel12InitCylinderEv(ShadowModel*);
extern "C" void func_ov006_020c057c(void*);

struct V3 { int x, y, z; };
struct V3s { short x, y, z; };
extern V3 data_020a0ebc;
extern V3s data_020a0edc;

extern "C" void func_ov006_020c092c(char* thiz)
{
    char* c = thiz;
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr*)c);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0x18), bmd, 1, -1);
    }
    _ZN9Animation8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 0x10));
    _ZN9Animation8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 8));
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        (BlendModelAnim*)(c + 0x18), **(BCA_File**)(c + 0xc), 0, 0, 0x800, 0);
    _ZN5Model12SetPolygonIDEi((Model*)(c + 0x18), 1);
    _ZN11ShadowModel12InitCylinderEv((ShadowModel*)(c + 0x88));
    *(int*)(c + 0xbc) = 0x1000;
    *(int*)(c + 0xc0) = 0x1000;
    *(int*)(c + 0xc4) = 0x1000;
    *(int*)(c + 0xc8) = data_020a0ebc.x;
    *(int*)(c + 0xcc) = data_020a0ebc.y;
    *(int*)(c + 0xd0) = data_020a0ebc.z;
    *(short*)(c + 0xe8) = data_020a0edc.x;
    *(short*)(c + 0xea) = data_020a0edc.y;
    *(short*)(c + 0xec) = data_020a0edc.z;
    func_ov006_020c057c(c);
}
