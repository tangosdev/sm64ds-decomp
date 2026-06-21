/* func_02005064 (true start 0x02005060) */

typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;

struct Camera {
    char pad[0x110];
    void *owner;         /* 0x110 Actor* */
    char pad2[0x28];     /* 0x114..0x13b */
    void *defaultCamDef; /* 0x13c */
    char pad3[0x3a];     /* 0x140..0x179 */
    s16  fov;            /* 0x17a */
    s16  unk17c;         /* 0x17c */
    char pad4[6];        /* 0x17e..0x183 */
    s16  unk184;         /* 0x184 */
};

struct Actor {
    char pad[0x8e];
    s16 angY;  /* 0x8e: Actor::ang.y */
};

extern void *glob_020874cc;

int func_02005064(struct Camera *self)
{
    struct Actor *owner;
    s16 a;
    s16 b;

    self->defaultCamDef = &glob_020874cc;
    owner = (struct Actor*)self->owner;
    b = self->unk17c;
    a = owner->angY;
    a = (s16)(a + 0x8000);
    self->unk184 = a - b;
    return 1;
}
