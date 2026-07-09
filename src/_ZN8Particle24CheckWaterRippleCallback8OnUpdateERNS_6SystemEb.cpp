//cpp
typedef int Fix12i;

namespace Particle {
struct Node;
struct System {
    int f0[7];
    int f1c;
    int f20, f24;
};
struct Node {
    Node *next;
    int f4;
    int f8, fc, f10, f14, f18, f1c, f20, f24;
    int f28;
    unsigned short f2c;
    unsigned short f2e;
};
struct CheckWaterRippleCallback {
    bool OnUpdate(System &sys, bool b);
};
}
extern "C" int data_0209f32c;
extern "C" void _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(Fix12i a, Fix12i b, Fix12i c);

bool Particle::CheckWaterRippleCallback::OnUpdate(Particle::System &sys, bool b)
{
    Particle::Node *node = (Particle::Node*)sys.f0[2];
    if (b) {
        *(int *)(((int)&sys + 0x1c) & 0xFFFFFFFFFFFFFFFF) &= ~2;
    } else {
        *(int *)(((int)&sys + 0x1c) & 0xFFFFFFFFFFFFFFFF) |= 2;
        if (node == 0) return false;
    }
    while (node != 0) {
        int sx = node->f14 + node->f8;
        int sy = node->f18 + node->fc;
        int sz = node->f1c + node->f10;
        if (node->f24 < 0 && (sy << 3) < data_0209f32c) {
            node->f2e = node->f2c;
            _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(sx << 3, data_0209f32c + 0x3000, sz << 3);
        }
        node = node->next;
    }
    return true;
}
