//cpp
typedef int s32;

extern void func_020395fc(void* c, void* a);
extern void func_020393fc(void* p, int v);
extern void* data_020a0c80[];

struct Actor;

struct MeshColliderBase {
    int Enable(Actor* a);
};

int MeshColliderBase::Enable(Actor* a)
{
    s32 i = 0;
    for (;;) {
        if (data_020a0c80[i] == 0) {
            func_020395fc(this, a);
            func_020393fc(this, i);
            data_020a0c80[i] = this;
            return 1;
        }
        i++;
        if (i >= 0x18) break;
    }
    return 0;
}
