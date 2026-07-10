//cpp
struct ModelComponents;
struct Vector3 { int x, y, z; };

struct TextureSequence {
    void Update(ModelComponents &);
};

struct Model {
    void Render(Vector3 const *);
};

extern "C" int _ZN7SkiLift6RenderEv(char *c) {
    ((TextureSequence *)(c + 0x138))->Update(*(ModelComponents *)(c + 0xdc));
    ((Model *)(c + 0xd4))->Render(0);
    return 1;
}
