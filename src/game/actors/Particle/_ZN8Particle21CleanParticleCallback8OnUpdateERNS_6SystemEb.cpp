//cpp
namespace Particle {
struct System {
    char pad[8];
    char *obj;
};
struct CleanParticleCallback {
    int OnUpdate(System &sys, bool done);
};
}

int Particle::CleanParticleCallback::OnUpdate(System &sys, bool done)
{
    char *q;
    if (!done) {
        q = sys.obj;
        while (q) {
            *(unsigned short*)(q + 0x2e) = *(unsigned short*)(q + 0x2c);
            return 0;
        }
    }
    return 1;
}
