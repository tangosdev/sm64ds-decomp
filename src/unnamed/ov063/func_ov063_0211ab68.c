typedef unsigned char u8;
typedef unsigned int u32;
typedef signed short s16;
typedef signed char s8;

struct Vec3 { int x, y, z; };
struct Vec3_16 { s16 x, y, z; };

extern int data_ov063_0211edc0;
extern struct Vec3 data_ov063_0211ee74;
extern struct Vec3 data_ov063_0211ee80;
extern struct Vec3 data_ov063_0211ee8c;
extern void* data_ov063_0211ee20;
extern void* data_ov063_0211edfc;
extern void* data_ov063_0211ee08;
extern int data_0209e650;
extern void func_020072c0(void);

extern void func_020731dc(struct Vec3* a, void (*b)(void), void** node);
extern u8 NumStars(void);
extern void _ZN9ActorBase18MarkForDestructionEv(char* o);
extern void Vec3_Add(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern int RandomIntInternal(int* seed);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, struct Vec3* pos, struct Vec3_16* rot, int a, int b);

void func_ov063_0211ab68(char* obj) {
    struct Vec3 pos;
    struct Vec3 tmp;
    struct Vec3_16 rot;
    int i;
    volatile int *p;

    if (!(data_ov063_0211edc0 & 1)) {
        /* first vec: plain fields → r4=0, r3=y, early arg loads, batch stores */
        data_ov063_0211ee74.x = 0;
        data_ov063_0211ee74.y = 0x32000;
        data_ov063_0211ee74.z = 0;
        func_020731dc(&data_ov063_0211ee74, func_020072c0, &data_ov063_0211ee20);

        /* second/third: volatile so stores interleave before arg loads, r3 holds shared xz */
        p = (volatile int *)&data_ov063_0211ee80;
        p[0] = 0xd2000;
        p[1] = 0x6e000;
        p[2] = 0xd2000;
        func_020731dc(&data_ov063_0211ee80, func_020072c0, &data_ov063_0211edfc);

        p = (volatile int *)&data_ov063_0211ee8c;
        p[0] = -0xd2000;
        p[1] = 0x46000;
        p[2] = -0xd2000;
        func_020731dc(&data_ov063_0211ee8c, func_020072c0, &data_ov063_0211ee08);
        data_ov063_0211edc0 |= 1;
    }

    if (NumStars() < 15) {
        _ZN9ActorBase18MarkForDestructionEv(obj);
        return;
    }

    for (i = 0; i < 3; i++) {
        Vec3_Add(&tmp, (struct Vec3*)(obj + 0x5c), &(&data_ov063_0211ee74)[i]);
        pos.x = tmp.x;
        pos.y = tmp.y;
        pos.z = tmp.z;
        rot.x = *(s16*)(obj + 0x92);
        rot.y = *(s16*)(obj + 0x94);
        rot.z = *(s16*)(obj + 0x96);
        rot.y = (u32)RandomIntInternal(&data_0209e650) >> 16;
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xd1, 0xfff6, &pos, &rot, *(s8*)(obj + 0x5d0), -1);
    }
    _ZN9ActorBase18MarkForDestructionEv(obj);
}
