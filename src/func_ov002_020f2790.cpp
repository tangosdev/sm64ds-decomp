//cpp
struct Fix12 { int v; };
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static System* New(unsigned int a, unsigned int b, int c, int d, int e,
                       const Vector3_16f* f, Callback* g);
};
}

extern "C" {

void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int c, int d, int e, const void* f, void* g);

void func_ov002_020f2790(char* self, int r1, int r2, void* r3, short s)
{
    *(void**)(self + 0x508) = r3;
    *(short*)(self + 0x50c) = s;
    *(void**)(self + 0x1fc) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(self + 0x1fc), 0x3e, r1 << 12, r2 << 12, 0,
            0, (void*)(self + 0x200));
}

}
