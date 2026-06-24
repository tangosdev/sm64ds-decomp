//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov070_02121880(void* c, int n);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void _ZN6Player16IncMegaKillCountEv(void* thiz);

void func_ov070_021211c4(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0x184);
    if (id == 0) return;
    char* o = (char*)_ZN5Actor10FindWithIDEj(id);
    if (o == 0) return;
    int b = (*(unsigned short*)(o + 0xc) == 0xbf);
    if (b == 0) return;
    int b2 = ((*(int*)(c + 0xb0) & 0x20000) != 0);
    if (b2 != 0) {
        func_ov070_02121880(c, 2);
        return;
    }
    if ((*(int*)(c + 0x180) & 0x10) == 0) return;
    *(short*)(c + 0x94) = Vec3_HorzAngle((Vector3*)(o + 0x5c), (Vector3*)(c + 0x5c));
    *(short*)(c + 0x8e) = (short)(*(short*)(c + 0x94) + 0x8000);
    _ZN6Player16IncMegaKillCountEv(o);
    func_ov070_02121880(c, 3);
}
}
