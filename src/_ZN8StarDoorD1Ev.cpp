//cpp
// @symbol _ZN4DoorD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct CommonModel { char pad[0x4]; ~CommonModel(); };

struct Door : Actor {
    CommonModel m0;   /* 0xd4 */
    virtual ~Door();
};

Door::~Door()
{
}
