//cpp
// @symbol _ZN9LightBeamD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x50]; ~Model(); };
struct MovingCylinderClsnWithPos { char pad[0x4]; ~MovingCylinderClsnWithPos(); };

struct LightBeam : Actor {
    Model m0;   /* 0xd4 */
    MovingCylinderClsnWithPos m1;   /* 0x124 */
    virtual ~LightBeam();
};

LightBeam::~LightBeam()
{
}
