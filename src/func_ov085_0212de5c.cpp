//cpp
typedef int Fix12;
typedef unsigned int u32;

struct PMF;
struct Player;
struct Actor {
    Player *ClosestPlayer();
    short HorzAngleToCPlayer();
};
namespace Sound { void PlaySub(u32, u32, u32, Fix12, bool); }
extern "C" void func_0201f32c(int);
extern "C" int func_ov085_0212e728(void *c, void *p);
extern unsigned char data_0209d66c;
extern int data_ov085_021307b0[];

struct PlayerObj {
    char pad8c[0x8c];
    short v8c, v8e, v90;
    char pad2c8[0x2c8 - 0x92];
    int v2c8;
    char pad2dc[0x2dc - 0x2cc];
    unsigned char v2dc;
};

struct Other {
    char pad8c[0x8c];
    short v8c, v8e, v90;
    char pad723[0x723 - 0x92];
    unsigned char v723;
};

extern "C" int func_ov085_0212de5c(PlayerObj *c)
{
    Other *p = (Other *)((Actor *)c)->ClosestPlayer();
    if (p == 0)
        return 1;
    {
        short t = c->v8e + 0x8000;
        p->v8c = 0;
        p->v8e = t;
        p->v90 = 0;
    }
    c->v8e = ((Actor *)c)->HorzAngleToCPlayer();
    c->v2dc = 0;
    c->v8c = 0x1000;
    c->v90 = 0x800;
    {
        int *pp = (int *)(((int)c + 0x2c8));
        int n = *pp + 1;
        *pp = n;
    }
    Sound::PlaySub(0x4b, 0x14, 0x7f, 0x15666, false);
    switch (c->v2c8) {
    case 1:
        p->v723 = 1;
        break;
    case 0x78:
        func_0201f32c(1);
        break;
    case 0x82:
        p->v723 = 0;
        break;
    case 0xb4:
        data_0209d66c = 1;
        break;
    case 0xd2:
        func_ov085_0212e728(c, data_ov085_021307b0);
        break;
    }
    return 1;
}
