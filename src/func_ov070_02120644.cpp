//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void _ZN6Player16IncMegaKillCountEv(void* thiz);
extern void func_ov070_02120da8(void* c, int n);
extern int _ZN6Player5ShockEj(void* thiz, unsigned int n);

int func_ov070_02120644(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0x1fc);
    if (id == 0) return 0;
    char* o = (char*)_ZN5Actor10FindWithIDEj(id);
    if (o == 0) goto ret0;
    {
        int b = (*(unsigned short*)(o + 0xc) == 0xbf);
        if (b != 0) goto cont;
    }
ret0:
    return 0;
cont:
    if (*(unsigned char*)(o + 0x6fb) != 0) return 0;
    if (*(int*)(c + 0x1f8) & 0x10) {
        *(short*)(c + 0x94) = Vec3_HorzAngle((Vector3*)(o + 0x5c), (Vector3*)(c + 0x5c));
        *(short*)(c + 0x8e) = (short)(*(short*)(c + 0x94) + 0x8000);
        _ZN6Player16IncMegaKillCountEv(o);
        func_ov070_02120da8(c, 2);
    } else {
        if (_ZN6Player5ShockEj(o, 1) != 0)
            func_ov070_02120da8(c, 0);
    }
    return 1;
}
}
