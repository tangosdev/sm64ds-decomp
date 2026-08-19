//cpp
// @symbol _ZN11SnowmanHeadD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct dCcAc_c { char pad[0x34]; ~dCcAc_c(); };
struct dBgCh_Actr { char pad[0x4]; ~dBgCh_Actr(); };

struct SnowmanHead : dActor_c {
    Model m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x124 */
    dCcAc_c m2;   /* 0x138 */
    dBgCh_Actr m3;   /* 0x16c */
    virtual ~SnowmanHead();
};

SnowmanHead::~SnowmanHead()
{
}
