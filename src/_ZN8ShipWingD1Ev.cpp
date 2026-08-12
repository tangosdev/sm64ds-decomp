//cpp
// @symbol _ZN18RotatingPlatformRrD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct CommonModel { char pad[0x4]; ~CommonModel(); };

struct RotatingPlatformRr : Actor {
    CommonModel m0;   /* 0xd4 */
    virtual ~RotatingPlatformRr();
};

RotatingPlatformRr::~RotatingPlatformRr()
{
}
