//cpp
// @symbol _ZN10PyramidTagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct dCcAc_c { char pad[0x4]; ~dCcAc_c(); };

struct PyramidTag : dActor_c {
    dCcAc_c m0;   /* 0xd4 */
    virtual ~PyramidTag();
};

PyramidTag::~PyramidTag()
{
}
