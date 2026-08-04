//cpp
struct SharedFilePtr; struct BMD_File;
struct Model { int d; };
struct ModelBase { int d; };
struct Animation { int d; };
extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void func_ov006_020c3adc(char *p);
extern "C" void func_ov006_020c3884(char *t, int a1);
extern "C" int func_ov006_020c3d88(char *c)
{
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 0xd7c));
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0xd18), bmd, 1, -1);
    }
    _ZN9Animation8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 0xd8c));
    _ZN9Animation8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 0xd94));
    _ZN9Animation8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 0xd9c));
    _ZN9Animation8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 0xda4));
    func_ov006_020c3adc(c);
    int m = (int)_ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr*)(c + 0xd84));
    int i = 0;
    int z = i;
    char *p = c + 8;
    for (; i < 0x16; i++) {
        func_ov006_020c3884(p, m);
        *(int *)(c + 0x48) = z;
        p += 0x98;
        c += 0x98;
    }
    return 1;
}
