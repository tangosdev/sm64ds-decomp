/* Particle::Callback::OnUpdate(Particle::System&, bool active) at 0x020226c8
 * Virtual hook on the nested Particle::Callback class (vtable slot 2), fired
 * each frame while a particle system is alive. The base returns true (1) to keep
 * the system updating; subclasses (SimpleCallback, BubbleCallback, ...) override.
 * `this` is Particle::Callback*; the System& reference becomes a pointer param.
 */

typedef int s32;

struct Particle_Callback; /* Particle::Callback (nested class) */
struct Particle_System;   /* Particle::System (nested class), passed by reference */

s32 _ZN8Particle8Callback8OnUpdateERNS_6SystemEb(
    struct Particle_Callback* self, struct Particle_System* system, _Bool active)
{
    (void)self; (void)system; (void)active;
    return 1;
}
