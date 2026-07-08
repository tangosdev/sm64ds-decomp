//cpp
extern "C" {
typedef long long s64;
struct Vec3 { int x, y, z; };
struct Vector3 { int x, y, z; };
struct Animation { void Advance(); };

void _Z14ApproachLinearRiii(int *val, int target, int step);
void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, Vector3 *v, unsigned int d);
unsigned char DecIfAbove0_Byte(unsigned char *p);
int func_ov070_02120644(char *c);
extern short SINE_TABLE[];

int func_ov070_021209e4(char *c) {
    _Z14ApproachLinearRiii((int*)(c + 0x424), 1000, 20);

    unsigned char bit0 = (unsigned char)(*(int*)(c + 8) & 1);
    if (bit0) {
        *(short*)(c + 0x94) = (short)(*(short*)(c + 0x94) + *(int*)(c + 0x424));
    } else {
        *(short*)(c + 0x94) = (short)(*(short*)(c + 0x94) - *(int*)(c + 0x424));
    }

    *(short*)(c + 0x42c) = (short)(*(short*)(c + 0x42c) + 0xa00);

    unsigned char bit1 = (unsigned char)(((unsigned int)*(int*)(c + 8) >> 1) & 1);
    int ip = bit1 ? 0xb4000 : 0x168000;

    *(int*)(c + 0x404) = *(int*)(c + 0x5c);
    *(int*)(c + 0x408) = *(int*)(c + 0x60);
    *(int*)(c + 0x40c) = *(int*)(c + 0x64);

    {
        int idx = (*(unsigned short*)(c + 0x94)) >> 4;
        int cosv = SINE_TABLE[idx * 2];
        *(int*)(c + 0x410) = (int)(((s64)ip * cosv + 0x800) >> 0xc);
    }
    {
        int idx = (*(unsigned short*)(c + 0x42c)) >> 4;
        int sinv = SINE_TABLE[idx * 2];
        *(int*)(c + 0x414) = (int)(((s64)0x14000 * sinv + 0x800) >> 0xc);
    }
    {
        int idx = (*(unsigned short*)(c + 0x94)) >> 4;
        int sinv = SINE_TABLE[idx * 2 + 1];
        *(int*)(c + 0x418) = (int)(((s64)ip * sinv + 0x800) >> 0xc);
    }

    AddVec3((Vec3*)(c + 0x404), (Vec3*)(c + 0x410), (Vec3*)(c + 0x404));

    *(short*)(c + 0x42e) = (short)(*(short*)(c + 0x42e) + 0x4000);

    {
        int idx = (*(unsigned short*)(c + 0x42e)) >> 4;
        int cosv = SINE_TABLE[idx * 2];
        int scale = (int)(((s64)0x555 * cosv + 0x800) >> 0xc) + 0x800;
        *(int*)(c + 0x80) = scale;
        *(int*)(c + 0x84) = scale;
        *(int*)(c + 0x88) = scale;
    }

    ((Animation*)(c + 0x124))->Advance();
    ((Animation*)(c + 0x188))->Advance();
    ((Animation*)(c + 0x19c))->Advance();

    *(unsigned int*)(c + 0x428) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c + 0x428), 3, 0x183, (Vector3*)(c + 0x74), 0);

    if (DecIfAbove0_Byte((unsigned char*)(c + 0x430)) == 0) {
        func_ov070_02120644(c);
    }
    return 1;
}
}
