//cpp
struct Actor;
struct MeshColliderBase { int IsEnabled(); void Disable(); void Enable(Actor *a); };

extern "C" int func_ov065_02118248(char *c)
{
    int b = (int)((*(int *)(c + 0xb0) & 8) != 0);
    if (b != 0) {
        int i;
        char *m = c + 0x180;
        for (i = 0; i < 7; i++) {
            if (((MeshColliderBase *)m)->IsEnabled())
                ((MeshColliderBase *)m)->Disable();
            m += 0x200;
        }
        return 1;
    }
    char *m = c + 0x180;
    int i;
    for (i = 0; i < 7; i++) {
        if (!((MeshColliderBase *)m)->IsEnabled())
            ((MeshColliderBase *)m)->Enable((Actor *)c);
        m += 0x200;
    }
    return 0;
}
