typedef unsigned char u8;
typedef unsigned short u16;
enum { false, true };
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov102_0214ae1c(char* a);
extern int func_ov002_020df34c(char* self);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int n, void* v);
void func_ov098_0213ad08(char* self) {
    unsigned int id = *(unsigned int*)(self + 0x148);
    char* a;
    char* p;
    int b1;
    if (id == 0) return;
    a = _ZN5Actor10FindWithIDEj(id);
    if (a == 0) return;
    b1 = *(u16*)(a + 0xc);
    b1 = b1 == 0xbf;
    if (b1 != false) {
        p = *(char**)(a + 0x358);
        if (p != 0) {
            int b2 = *(u16*)(p + 0xc);
            b2 = b2 == 0xce;
            if (b2 != false) {
                func_ov102_0214ae1c(p);
                return;
            }
        }
        if (func_ov002_020df34c(a) == 0) return;
        *(int*)(self + 0x180) = 3;
        *(u8*)(self + 0x185) = 0;
        *(char**)(self + 0x158) = a;
        *(int*)(((int)self + 0x13c) & 0xFFFFFFFFFFFFFFFF) |= 1;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x14b, self + 0x74);
    }
}
