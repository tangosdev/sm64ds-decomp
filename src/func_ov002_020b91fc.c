typedef unsigned char u8;
typedef unsigned int u32;
struct Vector3_16f;
struct Callback;
extern u8 DecIfAbove0_Byte(u8* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* p);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 id, u32 a, int x, int y, int z, const struct Vector3_16f* rot, struct Callback* cb);
typedef struct { int x, y, z; } V3;

void func_ov002_020b91fc(char* c)
{
    int f;
    V3 pos;

    do {
        if (*(int*)(c + 8) == 0xffff) break;
        if (DecIfAbove0_Byte((u8*)(c + 0x3ca)) != 0) break;
        f = *(int*)(c + 0xb0);
        if ((int)((f & 0x40000) != 0) != 0) break;
        if ((int)((f & 0x20000) != 0) != 0) break;
        _ZN9ActorBase18MarkForDestructionEv(c);
    } while (0);

    {
        int z = *(int*)(c + 0x64);
        int x = *(int*)(c + 0x5c);
        int y = *(int*)(c + 0x60) + 0x82000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        *(void**)(c + 0x3c4) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)(c + 0x3c4), 0x104,
            ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2],
            0, 0);
    }
}
