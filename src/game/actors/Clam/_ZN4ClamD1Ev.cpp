//cpp
// @symbol _ZN4ClamD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct dCcAc_c { char pad[0x4]; ~dCcAc_c(); };

struct Clam : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    dCcAc_c m1;   /* 0x138 */
    virtual ~Clam();
};

Clam::~Clam()
{
}
