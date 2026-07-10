//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };
struct Animation { int WillHitFrame(int) const; };
namespace Sound { void PlayBank0(unsigned int, const Vector3 &); }
struct Player {
    unsigned int GetBodyModelID(unsigned int, bool) const;
    void SetAnim(unsigned int, int, Fix12, unsigned int);
};

extern "C" int func_ov002_020c8f80(Player *thiz)
{
    char *p = (char *)thiz;

    *(int *)(((long long)(int)(p + 0x80)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x80;
    *(int *)(((long long)(int)(p + 0x84)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x80;
    *(int *)(((long long)(int)(p + 0x88)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x80;

    unsigned int id = thiz->GetBodyModelID(*(int *)(p + 8) & 0xff, false);
    Animation *anim = (Animation *)(*(char **)(p + id * 4 + 0xdc) + 0x50);
    if (anim->WillHitFrame(1)) {
        if (*(unsigned char *)(p + 0x70c) == 0) {
            *(unsigned char *)(p + 0x70c) = 1;
        } else {
            thiz->SetAnim(0x43, 0x40000000, 0x1000, 0);
            *(unsigned char *)(p + 0x6e3) = 0xd;
            *(int *)(p + 0x9c) = -0x4000;
            *(unsigned short *)(p + 0x8c) = 0x4000;
            Sound::PlayBank0(0x1a, *(Vector3 *)(p + 0x74));
        }
    }
    return 0;
}
