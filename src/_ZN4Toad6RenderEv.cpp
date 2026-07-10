//cpp
struct Vector3 { int x, y, z; };

struct Model {
    void Render(Vector3 const *);
};

extern "C" int _ZN4Toad6RenderEv(char *c) {
    ((Model *)(c + 0x108))->Render(0);
    return 1;
}
