typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
    int x, y, z;
} V3;

extern int _ZN6Player6IsAnimEj(void *self, unsigned int animId);
extern int _ZNK6Player14GetBodyModelIDEjb(void *self, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void *v);
extern u32 func_02022d00(u32 uniqueID, u32 effectID, int x, int y, int z, void *dir);
extern int data_0209f32c;

void func_ov002_020bf5e0(char *self)
{
    int flag;
    int id;
    char *anim;
    V3 out;
    V3 src;
    int target;
    int newId;
    int state;

    if (*(u8 *)(self + 0x703) == 0)
        return;

    flag = 0;
    if (_ZN6Player6IsAnimEj(self, 0x9e) || _ZN6Player6IsAnimEj(self, 0xa4)) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0xe))
            goto set_flag;
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (!_ZNK9Animation12WillHitFrameEi(anim, 0x1c))
            goto flag_done;
set_flag:
        flag = 1;
    }
flag_done:
    if (!flag)
        return;

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0x1c)) {
        src.x = 0x40000;
    } else {
        src.x = -0x40000;
    }
    src.y = 0x1b000;
    src.z = 0x40000;

    Vec3_RotateYAndTranslate(&out.x, (int *)(self + 0x5c), *(short *)(self + 0x8e), &src.x);

    if (*(u8 *)(self + 0x707) != 0 || *(u8 *)(self + 0x706) != 0) {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x64, out.x, out.y, out.z);
    } else {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x63, out.x, out.y, out.z);
    }

    _ZN5Sound9PlayBank0EjRK7Vector3(0xd0, self + 0x74);

    if (*(u8 *)(self + 0x707) != 0 && *(u8 *)(self + 0x706) == 0) {
        newId = func_02022d00(*(u32 *)(self + 0x628), 0x62, *(int *)(self + 0x5c), data_0209f32c, *(int *)(self + 0x64), 0);
        *(u32 *)(self + 0x628) = newId;
    }

    state = *(int *)(self + 0x66c);
    target = -1;
    if (state == 6)
        goto state_case6;
    if (state == 8)
        goto state_case8;
    if (state == 9) {
        target = 0xe6;
    }
    goto state_merge;
state_case8:
    target = 0xe4;
    goto state_merge;
state_case6:
    target = 0xe5;
state_merge:
    ;

    if (target == -1)
        return;

    src.y = 0;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(target, out.x, out.y, out.z);
}
