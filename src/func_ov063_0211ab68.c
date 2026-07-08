// NONMATCHING: different op / idiom (div=16). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
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
extern int RNG_STATE;
extern void func_020072c0(void);

extern void func_020731dc(struct Vec3* a, void (*b)(void), void** node);
extern u8 NumStars(void);
extern void MarkForDestruction(char* o);
extern void Vec3_Add(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern int RandomIntInternal(int* seed);
extern void Actor_Spawn(u32 id, u32 param, struct Vec3* pos, struct Vec3_16* rot, int a, int b);

void func_ov063_0211ab68(char* obj) {
    struct Vec3 pos;
    struct Vec3 tmp;
    struct Vec3_16 rot;
    int i;

    if (!(data_ov063_0211edc0 & 1)) {
        data_ov063_0211ee74.x = 0;
        data_ov063_0211ee74.y = 0x32000;
        data_ov063_0211ee74.z = 0;
        func_020731dc(&data_ov063_0211ee74, func_020072c0, &data_ov063_0211ee20);
        data_ov063_0211ee80.x = 0xd2000;
        data_ov063_0211ee80.y = 0x6e000;
        data_ov063_0211ee80.z = 0xd2000;
        func_020731dc(&data_ov063_0211ee80, func_020072c0, &data_ov063_0211edfc);
        data_ov063_0211ee8c.x = -0xd2000;
        data_ov063_0211ee8c.y = 0x46000;
        data_ov063_0211ee8c.z = -0xd2000;
        func_020731dc(&data_ov063_0211ee8c, func_020072c0, &data_ov063_0211ee08);
        data_ov063_0211edc0 |= 1;
    }

    if (NumStars() < 15) {
        MarkForDestruction(obj);
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
        rot.y = (u32)RandomIntInternal(&RNG_STATE) >> 16;
        Actor_Spawn(0xd1, 0xfff6, &pos, &rot, *(s8*)(obj + 0x5d0), -1);
    }
    MarkForDestruction(obj);
}
