typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;
typedef int s32;
typedef struct { s32 x, y, z; } Vector3;
enum { false, true };

extern void DecIfAbove0_Short(void *p);
extern void func_02012790(u32 code);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern s32 AngleDiff(s32 a, s32 b);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern char *_ZN5Actor15FindWithActorIDEjPS_(u32 actorId, void *prev);
extern void _ZN5Actor13SpawnSoundObjEj(void *self, u32 id);
extern void func_02012694(u32 code, void *pos);
extern void _ZN6Player5ShockEj(void *player, u32 flag);
extern void func_ov064_0211a6ec(void *c, int i);

void func_ov064_0211a4c4(char *sl)
{
    DecIfAbove0_Short(sl + 0x170);

    if (*(u16 *)(sl + 0x170) == 0x58)
        func_02012790(0xe);

    u32 id = *(u32 *)(sl + 0x15c);
    if (id == 0)
        return;
    if (*(u16 *)(sl + 0x170) != 0)
        return;

    char *fp = _ZN5Actor10FindWithIDEj(id);
    if (fp == 0)
        return;

    {
        int t = *(u16 *)(fp + 0xc);
        t = t == 0xbf;
        if (t != false) {
            Vector3 *psrc = (Vector3 *)(((long long)(int)(fp + 0x5c)));
            Vector3 v;
            v.x = psrc->x;
            v.y = psrc->y;
            v.z = psrc->z;
            if (AngleDiff(Vec3_HorzAngle(sl + 0x5c, &v), *(s16 *)(sl + 0x8e)) < 0x4000) {
                int sbcount = 0;
                int count = 0;
                char *cur = _ZN5Actor15FindWithActorIDEjPS_(0xd, 0);
                while (cur != 0) {
                    count++;
                    if (cur != sl) {
                        s32 fv = *(s32 *)(cur + 0x16c);
                        int r1v = 1;
                        if (fv != 1) {
                            if (fv != 2)
                                r1v = 0;
                        }
                        if (r1v != 0)
                            sbcount++;
                    }
                    cur = _ZN5Actor15FindWithActorIDEjPS_(0xd, cur);
                }

                if (sbcount + 1 == *(u8 *)(sl + 0x172)) {
                    if (count == *(u8 *)(sl + 0x172)) {
                        _ZN5Actor13SpawnSoundObjEj(sl, 0);
                        *(u8 *)(sl + 0x173) = 1;
                    } else {
                        func_02012790(0x26);
                    }
                    if (*(u8 *)(fp + 0x706) != 0)
                        func_02012694(0x22, sl + 0x74);
                    else
                        func_02012694(0x20, sl + 0x74);
                    func_ov064_0211a6ec(sl, 1);
                } else {
                    *(u16 *)(sl + 0x170) = 0x5a;
                    if (*(u8 *)(fp + 0x6f9) != 0)
                        _ZN6Player5ShockEj(fp, 0);
                    else
                        _ZN6Player5ShockEj(fp, 1);

                    for (;;) {
                        cur = _ZN5Actor15FindWithActorIDEjPS_(0xd, cur);
                        if (cur == 0)
                            break;
                        if (cur != sl)
                            func_ov064_0211a6ec(cur, 0);
                    }
                }
            }
        }
    }
}
