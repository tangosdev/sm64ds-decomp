//cpp
extern "C" {
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID,
    int x, int y, int z, const void* dir, void* callback);
}

extern unsigned char data_0209f2d8;

struct Vec3 { int x, y, z; };

extern "C" void func_ov002_020aeee4(char* c) {
    int t1 = (*(unsigned short*)(c + 0xc) == 0x115);
    unsigned int effectID = 0;
    if (t1 != false) effectID = 0x108;

    int t2 = ((*(unsigned int*)(c + 0xb0) & 8) != 0);
    if (t2 != false) {
        int t3 = (data_0209f2d8 == 1);
        if (t3 == false) return;
    }

    Vec3 pos;
    pos.x = *(int*)(c + 0x5c);
    pos.y = *(int*)(c + 0x60);
    pos.z = *(int*)(c + 0x64);
    pos.y += 0x1e000;
    volatile Vec3* vp = &pos;
    *(unsigned int*)(c + 0x394) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile unsigned int*)(c + 0x394), effectID, vp->x, vp->y, pos.z, 0, 0);
}
