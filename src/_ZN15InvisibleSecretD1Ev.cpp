//cpp
// @symbol _ZN15InvisibleSecretD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x50]; ~Model(); };
struct TextureSequence { char pad[0x4]; ~TextureSequence(); };

struct InvisibleSecret : Actor {
    Model m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x124 */
    virtual ~InvisibleSecret();
};

InvisibleSecret::~InvisibleSecret()
{
}
