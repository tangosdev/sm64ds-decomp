//cpp
// @symbol _ZN12HealingHeartD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct dCcAc_c { char pad[0x4]; ~dCcAc_c(); };

struct HealingHeart : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    dCcAc_c m1;   /* 0x138 */
    virtual ~HealingHeart();
};

HealingHeart::~HealingHeart()
{
}
