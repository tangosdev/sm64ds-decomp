/* Particle::Callback::SpawnParticles(Particle::System&) at 0x020226d0
 * Virtual hook on the nested Particle::Callback class (vtable slot 1), giving a
 * callback the chance to emit particles into the system. The base does nothing;
 * subclasses (EndingStarGlitterCallback, ScaleCallback, ...) override.
 * `this` is Particle::Callback*; the System& reference becomes a pointer param.
 */

struct Particle_Callback; /* Particle::Callback (nested class) */
struct Particle_System;   /* Particle::System (nested class), passed by reference */

void _ZN8Particle8Callback14SpawnParticlesERNS_6SystemE(
    struct Particle_Callback* self, struct Particle_System* system)
{
    (void)self; (void)system;
}
