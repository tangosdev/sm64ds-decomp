typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };
struct M4x3 { int w[12]; };

extern int data_ov098_0213bf60[];
extern int data_ov098_0213bf64[];
extern int data_ov098_0213bf68[];

extern int func_ov002_020e496c(char* c);
extern int _ZN6Player14IsFrontSlidingEv(char* p);
extern int _ZN6Player17LostGrabbedObjectEv(char* p);
extern void Math_Function_0203b14c(char* dst, int a, int b, int c, int d);
extern char* _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(char* self, char* player, const struct Vector3* v);
extern void Matrix4x3_FromRotationZXYExt(void* m, int x, int y, int z);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char* self);

void func_ov098_02139850(char* self)
{
    char* obj = *(char**)(self + 0x5e4);
    int b;

    if (obj == 0) goto other;
    b = (int)((*(u32*)(self + 0xb0) & 0x4000) != 0);
    if (b == 0) goto other;
    if (*(int*)(obj + 0xc8) == 0) goto other;

    {
        char* r4 = (char*)func_ov002_020e496c(obj);
        int r5 = 0;
        if (_ZN6Player14IsFrontSlidingEv(*(char**)(self + 0x5e4)) != 0) r5 = 1;
        if (_ZN6Player17LostGrabbedObjectEv(*(char**)(self + 0x5e4)) != 0) {
            if (((u32)*(int*)(r4 + 0x58) << 4) >> 0x10 < 0xe) r5 = 1;
        }
        if (*(int*)(*(char**)(self + 0x5e4) + 8) == 2) {
            r5 = (r5 + 2) & 0xff;
        }
        Math_Function_0203b14c(self + 0x4f4, *(int*)((char*)data_ov098_0213bf60 + r5 * 0xc), 0x800, 0x3e8000, 4);
        Math_Function_0203b14c(self + 0x4f8, *(int*)((char*)data_ov098_0213bf64 + r5 * 0xc), 0x800, 0x3e8000, 4);
        Math_Function_0203b14c(self + 0x4fc, *(int*)((char*)data_ov098_0213bf68 + r5 * 0xc), 0x800, 0x3e8000, 4);
        {
            char* res = _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(self, *(char**)(self + 0x5e4), (struct Vector3*)(self + 0x4f4));
            *(struct M4x3*)(self + 0xf0) = *(struct M4x3*)res;
        }
        *(u32*)(((int)self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000000;
    }
    return;

other:
    {
        volatile int tmp[3];
        tmp[0] = *(int*)(self + 0x5c);
        tmp[1] = *(int*)(self + 0x60);
        tmp[2] = *(int*)(self + 0x64);
        tmp[1] = *(int*)(self + 0x60) - *(int*)(self + 0x5f4);
        Matrix4x3_FromRotationZXYExt(self + 0xf0, *(s16*)(self + 0x8c), *(s16*)(self + 0x8e), *(s16*)(self + 0x90));
        *(int*)(self + 0x114) = tmp[0] >> 3;
        *(int*)(self + 0x118) = tmp[1] >> 3;
        *(int*)(self + 0x11c) = tmp[2] >> 3;
        *(int*)(self + 0x4f4) = 0;
        *(int*)(self + 0x4f8) = 0;
        *(int*)(self + 0x4fc) = 0;
        if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x320) != 0) {
            *(u32*)(((int)self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x4000000;
        }
    }
}
