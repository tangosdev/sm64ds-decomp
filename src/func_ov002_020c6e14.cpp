//cpp
typedef unsigned char u8;
struct Vector3;
struct ActorBase;
extern "C" u8 NumStars(void);
extern u8 NEW_STAR_COLLECTED;
extern short data_ov002_020ff1a0[];
extern void* CAMERA;

struct ActorBase {};
struct Player : ActorBase {
    int ShowMessage(ActorBase& a, unsigned int b, const Vector3* c, unsigned int d, unsigned int e);
};
struct Camera { void SetFlag_3(); };

extern "C" int func_ov002_020c6e14(Player* self)
{
    int idx;

    if (NEW_STAR_COLLECTED != 0) {
        idx = -1;
        switch (NumStars()) {
        case 1:  idx = 0; break;
        case 3:  idx = 1; break;
        case 8:  idx = 2; break;
        case 12: idx = 3; break;
        case 30: idx = 4; break;
        case 50: idx = 5; break;
        case 80: idx = 6; break;
        }

        if (idx == -1)
            return 0;

        if (self->ShowMessage(*self, data_ov002_020ff1a0[idx], 0, 0, 2) != 0) {
            ((Camera*)CAMERA)->SetFlag_3();
            return 1;
        }
    }
    return 0;
}
