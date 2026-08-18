//cpp
// @symbol _ZN10PyramidTagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct PyramidTag : dActor_c {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~PyramidTag();
};

PyramidTag::~PyramidTag()
{
}
