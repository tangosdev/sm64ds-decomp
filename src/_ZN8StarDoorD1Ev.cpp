//cpp
// @symbol _ZN8StarDoorD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct CommonModel { char pad[0x4]; ~CommonModel(); };

struct StarDoor : Actor {
    CommonModel m0;   /* 0xd4 */
    virtual ~StarDoor();
};

StarDoor::~StarDoor()
{
}
