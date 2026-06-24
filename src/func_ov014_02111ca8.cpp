//cpp
typedef short s16;
extern "C" {
int func_ov014_02111f54(void* c);
void ApproachAngle(void* p, int a, int b, int c, int d);
unsigned short DecIfAbove0_Short(unsigned short* p);
void _Z14ApproachLinearRiii(int* p, int to, int step);
void _Z14ApproachLinearRsss(short* p, short to, short step);
int _ZN5Actor13DistToCPlayerEv(void* self);
short _ZN5Actor18HorzAngleToCPlayerEv(void* self);
int AngleDiff(int a, int b);
void func_ov014_02111ebc(void* c, int i);
void _ZN9Animation7AdvanceEv(void* self);

void func_ov014_02111ca8(char* c){
    if (func_ov014_02111f54(c)) return;
    ApproachAngle(c + 0x8c, 0, 4, 0x200, 0x80);
    DecIfAbove0_Short((unsigned short*)(c + 0x5fc));
    _Z14ApproachLinearRiii((int*)(c + 0x5f8), 0x50000, 0x1000);
    _Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x602), 0x190);
    _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x400);
    if (*(unsigned char*)(c + 0x61c)) {
        int d = _ZN5Actor13DistToCPlayerEv(c);
        *(short*)(c + 0x602) = _ZN5Actor18HorzAngleToCPlayerEv(c);
        *(short*)(c + 0x94) = *(short*)(c + 0x8e);
        *(int*)(c + 0x98) = 0xa000;
        *(int*)(c + 0xa8) = 0x14000;
        *(short*)(c + 0x600) = 0;
        if (d < 0x500000 &&
            AngleDiff(*(short*)(c + 0x602), *(short*)(c + 0x8e)) < 0x800 &&
            *(unsigned short*)(c + 0x5fc) == 0) {
            func_ov014_02111ebc(c, 2);
        }
    }
    _ZN9Animation7AdvanceEv(c + 0x1a0);
}
}
