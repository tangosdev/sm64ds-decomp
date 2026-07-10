// NONMATCHING: base materialization / addressing (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Vector3_16f;
struct Callback;
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 id, u32 a, int x, int y, int z, const struct Vector3_16f* rot, struct Callback* cb);

extern u8 data_0209f2d8;

void func_ov002_020aeee4(char* c)
{
    u32 id;
    int x, y, z;
    int flag;

    flag = (int)(*(u16*)(c + 0xc) == 0x115);
    id = 0;
    if (flag != 0) id = 0x108;

    if ((int)((*(int*)(c + 0xb0) & 8) != 0) != 0) {
        int b = (int)(data_0209f2d8 == 1);
        if (b == 0) return;
    }

    x = *(int*)(c + 0x5c);
    y = *(int*)(c + 0x60);
    z = *(int*)(c + 0x64);
    y = *(int*)(c + 0x60) + 0x1e000;
    *(void**)(c + 0x394) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x394), id, x, y, z, 0, 0);
}
