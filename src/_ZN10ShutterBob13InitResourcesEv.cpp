//cpp
class Actor {};
class MeshColliderBase {
public:
    void Enable(Actor *a);
};

extern int func_ov002_020bad10(void *c, void **f);
extern int data_ov014_021145c4;

extern "C" int _ZN10ShutterBob13InitResourcesEv(char *c) {
    int r4 = func_ov002_020bad10(c, (void **)&data_ov014_021145c4);
    ((MeshColliderBase *)(c + 0x124))->Enable((Actor *)c);
    return r4;
}
