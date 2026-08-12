//cpp
extern "C" {
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int x, int y, int z, const void* v, void* cb);
int _ZN18PoppingLavaBubbles8BehaviorEv(char* c) {
    *(void**)(c+0xd8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned*)(c+0xd8), *(unsigned short*)(c+0xd4),
        *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64), 0, 0);
    return 1;
}
}
