//cpp
// @symbol _ZN6NumberD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct TextureSequence { char pad[0x4]; ~TextureSequence(); };

struct Number : dActor_c {
    Model m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x124 */
    virtual ~Number();
};

Number::~Number()
{
}
