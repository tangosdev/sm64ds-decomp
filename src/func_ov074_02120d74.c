#define false 0

typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern short SINE_TABLE[];

extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN6Player12Unk_020c6a10Ej(char *p, unsigned int a);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char *p, const struct Vector3 *v, unsigned int a, int b, unsigned int c, unsigned int d, unsigned int e);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern int AngleDiff(int a, int b);
extern void func_ov074_02121a4c(char *c, int idx);
extern void func_ov074_021203e4(char *c, int i);
extern void func_ov074_02120bb8(char *a, char *b, char *c, int d);
extern void func_02012694(unsigned int a, void *b);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, const struct Vector3 *, const void *, int, int);

void func_ov074_02120d74(char *a)
{
    struct Vector3 v0;
    struct Vector3 v1;
    struct Vector3 v2;
    struct Vector3 v3;
    int i;
    char *player;
    char *cur;
    char *p;
    int hit;
    int dir;
    int dir2;
    u32 s;
    u32 flags;
    u16 type;
    int b;
    int b2;

    DecIfAbove0_Byte((u8 *)(a + 0x605));
    s = *(u32 *)(a + 0x5cc);
    if (s == 1)
        return;
    if (s == 2)
        return;
    if (s - 5 <= 3)
        return;

    player = 0;
    i = 0;
    p = a + 0x110;
    do {
        cur = p;
        if (*(u32 *)(cur + 0x24) != 0)
            player = _ZN5Actor10FindWithIDEj(*(u32 *)(cur + 0x24));
        if (player != 0)
            break;
        i++;
        p += 0x40;
    } while (i < 4);
    if (player == 0)
        return;

    flags = *(u32 *)(cur + 0x20);
    hit = 0;
    if (flags & 0x2000) {
        hit = 1;
    } else {
        type = *(u16 *)(player + 0xc);
        b = (int)(type == 0xbf);
        if (b != 0) {
            if (*(u8 *)(a + 0x605) == 0) {
                if (!(flags & 0x26fe0) && *(u8 *)(player + 0x708) == 0) {
                    if (cur == a + 0x190) {
                        if (*(s16 *)(a + 0x5f6) != 0) {
                            _ZN6Player12Unk_020c6a10Ej(player, 2);
                            *(u8 *)(a + 0x605) = 0x1e;
                        } else {
                            v1.x = *(int *)(a + 0x5c);
                            v1.y = *(int *)(a + 0x60);
                            v1.z = *(int *)(a + 0x64);
                            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v1, 1, 0x5000, 1, 0, 1);
                        }
                    } else if (cur == a + 0x1d0) {
                        if (*(s16 *)(a + 0x5f6) != 0) {
                            _ZN6Player12Unk_020c6a10Ej(player, 2);
                            *(u8 *)(a + 0x605) = 0x1e;
                        } else {
                            v2.x = *(int *)(a + 0x5c);
                            v2.y = *(int *)(a + 0x60);
                            v2.z = *(int *)(a + 0x64);
                            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v2, 1, 0x5000, 1, 0, 1);
                        }
                    } else {
                        v3.x = *(int *)(a + 0x5c);
                        v3.y = *(int *)(a + 0x60);
                        v3.z = *(int *)(a + 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v3, 1, 0x5000, 1, 0, 1);
                        *(u8 *)(a + 0x605) = 0x10;
                    }
                }
                if (*(u8 *)(player + 0x703) != 0) {
                    if (*(u32 *)(cur + 0x20) & 0x3c0) {
                        hit = 1;
                    } else {
                        int ang = Vec3_HorzAngle((struct Vector3 *)(a + 0x5c), (struct Vector3 *)(player + 0x5c));
                        dir = 1;
                        if (AngleDiff(ang, *(s16 *)(a + 0x8e)) > 0x4000)
                            dir = -1;
                        func_ov074_02121a4c(a, 7);
                        func_ov074_021203e4(a, 8);
                        func_ov074_02120bb8(a, player, cur, 0);
                        func_02012694(0x15a, a + 0x74);
                        *(s16 *)(a + 0x5f6) = dir * (*(int *)(a + 0x5f0) * -0xc8);
                    }
                } else {
                    if (cur == a + 0x150) {
                        if (*(u32 *)(cur + 0x20) & 0x80) {
                            if (*(u32 *)(a + 0x5cc) == 4)
                                func_ov074_021203e4(a, 2);
                            else
                                func_ov074_021203e4(a, 4);
                            if (*(int *)(player + 8) == 0) {
                                v0.x = *(int *)(a + 0x5c);
                                v0.y = *(int *)(a + 0x60);
                                v0.z = *(int *)(a + 0x64);
                                if (*(u32 *)(a + 0x5dc) == 0 && _ZN5Actor10FindWithIDEj(*(u32 *)(a + 0x5dc)) == 0) {
                                    int k = ((u16)(s16)(*(int *)(a + 0x5f0) * ((s16)((*(int *)(a + 0x80) / 0x1000) * 0x350 + 0x500) + 0x600) + (*(s16 *)(a + 0x5f4) + *(s16 *)(a + 0x5f8)))) >> 4;
                                    v0.x = (int)(((long long)SINE_TABLE[k * 2] * 0x546000LL + 0x800) >> 12);
                                    v0.y += 0x32000;
                                    v0.z = (int)(((long long)SINE_TABLE[k * 2 + 1] * 0x546000LL + 0x800) >> 12);
                                    *(int *)(a + 0x5dc) = *(int *)(_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x115, 0, &v0, 0, *(s8 *)(a + 0xcc), -1) + 4);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            b2 = (int)(type == 0xc8);
            if (b2 != 0) {
                if (flags & 0x22000)
                    hit = 1;
            }
        }
    }
    if (hit == 0)
        return;
    func_02012694(0x15b, a + 0x74);
    {
    int ang2 = Vec3_HorzAngle((struct Vector3 *)(a + 0x5c), (struct Vector3 *)(player + 0x5c));
    dir2 = 1;
    if (AngleDiff(ang2, *(s16 *)(a + 0x8e)) > 0x4000) {
        func_ov074_02121a4c(a, dir2);
        dir2 = -1;
    } else {
        func_ov074_02121a4c(a, 2);
    }
    }
    *(s16 *)(a + 0x5f6) = dir2 * (*(int *)(a + 0x5f0) * -0x190);
    func_ov074_021203e4(a, 5);
    func_ov074_02120bb8(a, player, cur, 1);
}
