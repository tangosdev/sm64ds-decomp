/* Enemy::~Enemy() at 0x020aed74 (ov002) -- complete-object destructor (D1).
 * Sets the vptr to Enemy's vtable, then delegates to the Actor base-object
 * destructor and returns this. Enemy (0x110) : Actor (0xd4) : ActorBase.
 *
 * It occupies slot 16 of dEnemyBase_c's vtable, and a D2 is never in a vtable;
 * it was named D2 until notes/dtor-variant-audit.md. The Actor call below is a
 * genuine D2 -- that is what a D1 delegates to.
 */

extern void *data_ov002_021081e4;                 /* Enemy vtable (wildcard reloc) */
extern void *_ZN5ActorD2Ev(void *self);  /* Actor::~Actor (already named) */

struct Enemy {
    void *vtable;
};

void *_ZN5EnemyD1Ev(struct Enemy *self)
{
    self->vtable = &data_ov002_021081e4;
    _ZN5ActorD2Ev(self);
    return self;
}
