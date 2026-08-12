//cpp
// @symbol _ZN11VirtualDoorD1Ev

struct Actor {
    virtual ~Actor();
};


struct VirtualDoor : Actor {
    virtual ~VirtualDoor();
};

VirtualDoor::~VirtualDoor()
{
}
