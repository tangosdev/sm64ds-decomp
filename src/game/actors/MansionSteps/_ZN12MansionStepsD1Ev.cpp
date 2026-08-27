//cpp
// @symbol _ZN12MansionStepsD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x88]; ~Model(); };
struct dBgW_KcMbg { char pad[0x4]; ~dBgW_KcMbg(); };

struct MansionSteps : dActor_c {
    Model m0;   /* 0xd4 */
    dBgW_KcMbg m1;   /* 0x15c */
    virtual ~MansionSteps();
};

MansionSteps::~MansionSteps()
{
}
