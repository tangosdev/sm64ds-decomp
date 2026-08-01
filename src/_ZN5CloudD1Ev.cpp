//cpp
// @symbol _ZN5CloudD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x4]; ~Model(); };

struct Cloud : Actor {
    Model m0;   /* 0xd4 */
    virtual ~Cloud();
};

Cloud::~Cloud()
{
}
