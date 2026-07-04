//cpp
struct Vector3 { int x, y, z; };
extern "C" int _ZN6Player6IsAnimEj(void*, unsigned int);
extern "C" int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, bool);
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, const Vector3&);

extern "C" int func_ov002_020cb354(void* thisptr)
{
    unsigned char* p = (unsigned char*)thisptr;
    int a = _ZN6Player6IsAnimEj(thisptr, 0x24);
    if (a == 0) return a;
    int idx = _ZNK6Player14GetBodyModelIDEjb(thisptr, *(unsigned int*)(p + 8) & 0xff, 0);
    int* m = *(int**)(p + (idx << 2) + 0xdc);
    int* q = (int*)(((int)m + 0x50) & 0xFFFFFFFFFFFFFFFF);
    if ((int)((unsigned int)((q[2]) << 4) >> 0x10) < 8) {
        unsigned char f = *(unsigned char*)(p + 0x70c);
        if (f != 0) return f;
        if (*(int*)(*(int*)(p + 0x37c) + 0x18) & 0x1000000)
            _ZN5Sound9PlayBank0EjRK7Vector3(0xd, *(Vector3*)(p + 0x74));
        else
            _ZN5Sound9PlayBank0EjRK7Vector3(0xb0, *(Vector3*)(p + 0x74));
        *(unsigned char*)(p + 0x70c) = 1;
        return 1;
    }
    *(unsigned char*)(p + 0x70c) = 0;
    return 0;
}
