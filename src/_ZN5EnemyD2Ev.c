/* Enemy::~Enemy() at 0x020aed74 (ov002) -- base-object destructor.
 * Sets the vptr to Enemy's vtable, then delegates to the Actor base-object
 * destructor and returns this. Enemy (0x110) : Actor (0xd4) : ActorBase.
 */

extern void *_ZTV5Enemy;                 /* Enemy vtable (wildcard reloc) */
extern void *_ZN5ActorD2Ev(void *self);  /* Actor::~Actor (already named) */

struct Enemy {
    void *vtable;
};

void *_ZN5EnemyD2Ev(struct Enemy *self)
{
    self->vtable = &_ZTV5Enemy;
    _ZN5ActorD2Ev(self);
    return self;
}
