//cpp
typedef int Fix12;
struct MeshColliderBase { bool IsEnabled(); void Disable(); };
struct Platform {
    void UpdateModelPosAndRotY();
    bool IsClsnInRange(Fix12 a, Fix12 b);
    void UpdateClsnPosAndRot();
};

extern "C" int _ZN10DonutBlock8BehaviorEv(char *c)
{
    short *s = (short *)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);
    short *t = (short *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFF);
    *s = (short)(*s + t[0x1e / 2]);
    int b = (int)((*(int *)(c + 0xb0) & 8) != 0);
    if (b != 0) {
        if (((MeshColliderBase *)(c + 0x124))->IsEnabled())
            ((MeshColliderBase *)(c + 0x124))->Disable();
        return 1;
    }
    Platform *p = (Platform *)c;
    p->UpdateModelPosAndRotY();
    if (p->IsClsnInRange(0, 0))
        p->UpdateClsnPosAndRot();
    return 1;
}
