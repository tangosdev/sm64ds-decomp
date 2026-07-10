//cpp
struct ShadowModel { ~ShadowModel(); };
struct Model { ~Model(); };
struct ModelAnim { ~ModelAnim(); };
struct Actor { ~Actor(); };

extern int _ZTV11MirrorLuigi;
extern int _ZN15TextureSequenceD1Ev;
extern "C" int func_0207328c(char *, int, int, int);

extern "C" int _ZN11MirrorLuigiD1Ev(char *c) {
    *(int *)c = (int)&_ZTV11MirrorLuigi;
    func_0207328c(c + 0x1b0, 2, 0x14, (int)&_ZN15TextureSequenceD1Ev);
    ((ShadowModel *)(c + 0x188))->~ShadowModel();
    ((Model *)(c + 0x138))->~Model();
    ((ModelAnim *)(c + 0xd4))->~ModelAnim();
    ((Actor *)c)->~Actor();
    return (int)c;
}
