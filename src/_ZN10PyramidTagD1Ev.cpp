//cpp
// @symbol _ZN10PyramidTagD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct PyramidTag : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~PyramidTag();
};

PyramidTag::~PyramidTag()
{
}
