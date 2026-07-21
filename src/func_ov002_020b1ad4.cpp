//cpp
typedef unsigned char u8;
typedef int Fix12i;

struct Vector3 { int x, y, z; };

struct Bits3ae { u8 lo : 2; u8 field : 3; u8 hi : 3; };

extern signed char data_0209f2f8;

extern "C" {
void func_ov002_020b1ad4(char* self);
char* _ZN5Actor13ClosestPlayerEv(char* self);
Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
void _ZN12WithMeshClsn13SetLimMovFlagEv(char* self);
int _ZNK12WithMeshClsn13JustHitGroundEv(char* self);
int* _ZNK12WithMeshClsn14GetFloorResultEv(char* self);
int func_02037e78(int *p);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, Vector3 const &);
void func_ov002_020b13e0(char* self);
void func_ov002_020b1384(char* c);
}

extern "C" void func_ov002_020b1ad4(char* self) {
    char* player;
    int r6;
    int v;
    v = ((Bits3ae*)(self + 0x3ae))->field;
    if (v == 0) {
        player = _ZN5Actor13ClosestPlayerEv(self);
        if (player == 0) return;
        r6 = (data_0209f2f8 == 0xb) ? 0x258000 : 0x4b0000;
        if (Vec3_Dist((Vector3*)(self + 0x5c), (Vector3*)(player + 0x5c)) > r6) return;
        v = *(int*)(player + 0x98);
        if (v < 0x14000) v = 0x14000;
        else if (v > 0x28000) v = 0x28000;
        if (data_0209f2f8 == 0xb) v <<= 1;
        *(int*)(self + 0x98) = v;
        *(int*)(self + 0xa8) = 0x14000;
        {
            Bits3ae* b = (Bits3ae*)((unsigned long long)(self + 0x3ae) & 0xFFFFFFFFFFFFFFFFULL);
            v = b->field;
            b->field = (u8)(v + 1);
        }
        _ZN12WithMeshClsn13SetLimMovFlagEv(self + 0x1ac);
        *(short*)(self + 0x3a8) = 0x1c2;
    }
    if (_ZNK12WithMeshClsn13JustHitGroundEv(self + 0x1ac)) {
        if (func_02037e78(_ZNK12WithMeshClsn14GetFloorResultEv(self + 0x1ac) + 1) == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x52, *(Vector3*)(self + 0x74));
            *(int*)(self + 0xa8) = 0x19000;
        }
    }
    func_ov002_020b13e0(self);
    func_ov002_020b1384(self);
}
