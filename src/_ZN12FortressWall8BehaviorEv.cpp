//cpp
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };

struct MeshColliderBase {
    int IsEnabled();
    void Disable();
};

struct Actor;
namespace Sound { int PlaySecretSound(Actor *a, unsigned short *p); }

struct Actor {
    static Actor *Spawn(unsigned int id, unsigned int param, const Vector3 &pos, const Vector3_16 *rot, int a, int b);
};
struct ActorBase {
    void MarkForDestruction();
};

extern "C" void func_020393a4(int *p, int v);

struct Platform : ActorBase {
    int IsClsnInRange(int a, int b);
};

extern "C" int _ZN12FortressWall8BehaviorEv(Platform *self) {
    int ok = (*(unsigned short *)((char *)self + 0xc) == 0x30);
    if (ok != 0 && *(unsigned char *)((char *)self + 0x321) != 0) {
        if (((MeshColliderBase *)((char *)self + 0x124))->IsEnabled() != 0) {
            ((MeshColliderBase *)((char *)self + 0x124))->Disable();
        }
        if (Sound::PlaySecretSound((Actor *)self, (unsigned short *)((char *)self + 0x322)) != 0) {
            Vector3 pos;
            pos.x = *(int *)((char *)self + 0x5c);
            pos.y = *(int *)((char *)self + 0x60);
            pos.z = *(int *)((char *)self + 0x64);
            pos.y += 0xc8000;
            Actor::Spawn(0xb2, *(unsigned char *)((char *)self + 0x31f) | 0x40,
                         pos, (Vector3_16 *)0, *(signed char *)((char *)self + 0xcc), -1);
            ((ActorBase *)self)->MarkForDestruction();
        }
        return 1;
    }
    func_020393a4((int *)((char *)self + 0x124), 0x240000);
    self->IsClsnInRange(0x240000, 0);
    return 1;
}
