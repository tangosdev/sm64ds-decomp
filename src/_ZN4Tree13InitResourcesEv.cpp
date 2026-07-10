//cpp
struct Vector3 { int x, y, z; };
struct CylinderClsnWithPos {
    void Init(const Vector3&, int, int, unsigned int, unsigned int);
};
struct Model { void LoadAndSetFile(unsigned short, int, int); };
extern "C" void* _Znwj(unsigned int);
extern "C" void _ZN19CylinderClsnWithPosC1Ev(void*);
extern "C" void Vec3_AsrInPlace(void*, int);
extern int data_ov002_02110a48[];
extern unsigned short data_ov002_0210abb8[];

extern "C" int _ZN4Tree13InitResourcesEv(char* self);
int _ZN4Tree13InitResourcesEv(char* self) {
    int idx = ((unsigned int)*(int*)(self + 8) >> 4) & 7;
    int* slot;
    char* p;
    if (idx >= 4) idx = 4;
    slot = &data_ov002_02110a48[idx];
    if (*slot == 0) {
        ((Model*)(self + 0xd4 + idx * 0x50))->LoadAndSetFile(data_ov002_0210abb8[idx], 1, 1);
    }
    p = (char*)_Znwj(0x4c);
    if (p) _ZN19CylinderClsnWithPosC1Ev(p + 0xc);
    *(int*)(p + 0) = *(int*)(self + 0x5c);
    *(int*)(p + 4) = *(int*)(self + 0x60);
    *(int*)(p + 8) = *(int*)(self + 0x64);
    Vec3_AsrInPlace(p, 3);
    {
        int* q = (int*)(((int)p + 4) & 0xFFFFFFFFFFFFFFFF);
        *q = *q + 0x1e000;
    }
    ((CylinderClsnWithPos*)(p + 0xc))->Init(*(Vector3*)(self + 0x5c), 0x35555, 0x1f4000, 0x380000c, 0);
    *(int*)(p + 0x48) = *slot;
    *slot = (int)p;
    if (*(int*)(p + 0x48) != 0) {
        return 0;
    } else {
        return 1;
    }
}
