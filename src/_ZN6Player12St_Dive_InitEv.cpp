//cpp
// Player::St_Dive_Init - play a bank sound + random char voice, start the dive anim,
// bump vertical velocity with a clamp, then clear a run of state byte flags. Sibling of
// St_SlideKick_Init.
struct Vector3 { int x, y, z; };
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, const Vector3 &);
extern "C" int RandomIntInternal(int *seed);
extern "C" void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, const Vector3 &);
extern "C" void _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned int, int, int, unsigned int);
extern int data_ov002_0210e160;
extern int data_ov002_020ff130[];

extern "C" int _ZN6Player12St_Dive_InitEv(void *thisptr)
{
    unsigned char *p = (unsigned char *)thisptr;
    _ZN5Sound9PlayBank0EjRK7Vector3(0x11, *(Vector3 *)(p + 0x74));
    unsigned int r = (unsigned int)RandomIntInternal(&data_ov002_0210e160);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(p[0x6d9], data_ov002_020ff130[(r >> 4) & 1], *(Vector3 *)(p + 0x74));
    _ZN6Player7SetAnimEji5Fix12IiEj(thisptr, 0x40, 0x40000000, 0x1000, 0);
    *(unsigned char *)(p + 0x6e3) = 0;
    int *yv = (int *)(((int)p + 0x98) & 0xFFFFFFFFFFFFFFFF);
    *(int *)(p + 0xa8) = 0x1e000;
    yv[0] += 0xf000;
    if (*(int *)(p + 0x98) > 0x28000) *(int *)(p + 0x98) = 0x28000;
    *(unsigned char *)(p + 0x6de) = 1;
    *(unsigned char *)(p + 0x6df) = 0;
    *(unsigned char *)(p + 0x6e5) = 0;
    *(unsigned char *)(p + 0x6e6) = 0;
    *(unsigned char *)(p + 0x6e7) = 0;
    *(unsigned char *)(p + 0x6e4) = 0;
    *(unsigned char *)(p + 0x70c) = *(unsigned char *)(p + 0x70e);
    return 1;
}
