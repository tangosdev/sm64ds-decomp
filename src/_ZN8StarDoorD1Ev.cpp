//cpp
// @symbol _ZN8StarDoorD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct CommonModel { char pad[0x4]; ~CommonModel(); };

struct StarDoor : dActor_c {
    CommonModel m0;   /* 0xd4 */
    virtual ~StarDoor();
};

StarDoor::~StarDoor()
{
}
