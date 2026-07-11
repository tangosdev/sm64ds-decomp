typedef unsigned int u32;
typedef int Fix12i;
typedef short s16;

extern s16 Vec3_HorzAngle(const void* v0, const void* v1);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void* v);
extern unsigned short data_ov002_020ff01c[];

void func_ov002_020ae73c(char* c, char* arg)
{
    *(s16*)(c + 0x94) = Vec3_HorzAngle(arg + 0x5c, c + 0x5c);
    *(int*)(c + 0xa8) = 0xa000;

    if ((int)(*(unsigned short*)(arg + 0xc) == 0xbf) != 0)
        *(int*)(c + 0x98) = (data_ov002_020ff01c[(unsigned short)*(u32*)(arg + 8)] << 15) / 50;
    else
        *(int*)(c + 0x98) = 0x14000;

    {
        int *p = (int *)(((int)(c) + 0x98) & 0xFFFFFFFFFFFFFFFF);
        *p = *p / 6;
    }
    *(s16*)(c + 0x102) = 8;
    *(int *)(((int)(c) + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~1;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xa, c + 0x74);
}
