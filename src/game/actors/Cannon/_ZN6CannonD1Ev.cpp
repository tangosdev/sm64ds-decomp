//cpp
// @symbol _ZN6CannonD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct dCcAc_c { char pad[0x4]; ~dCcAc_c(); };

struct Cannon : dActor_c {
    Model m0;   /* 0xd4 */
    dCcAc_c m1;   /* 0x124 */
    virtual ~Cannon();
};

Cannon::~Cannon()
{
}
