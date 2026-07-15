typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void func_0201267c(unsigned int a, void *p);
extern void _Z14ApproachLinearRsss(s16 *val, s16 target, s16 step);
extern int _ZN9Animation8FinishedEv(void *anim);
extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player16IncMegaKillCountEv(void *player);
extern void func_02012694(unsigned int a, void *p);
extern void func_ov084_0212ebb4(void *c);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);

void func_ov084_0212f6d8(char *c)
{
    void *actor;
    u16 type;
    int isPlayer;
    int isNine;
    u32 flags;
    struct Vector3 pos1;
    struct Vector3 pos2;

    *(u8 *)(c + 0x45c) = 1;

    if (_ZNK9Animation12WillHitFrameEi(c + 0x160, 0x10)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x20)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x34)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x4b)) {
        func_0201267c(0xc0, c + 0x74);
    }

    _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x468), 0x800);
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);

    if (_ZN9Animation8FinishedEv(c + 0x160) != 0) {
        int thr;
        if (*(int *)(c + 0x46c) != 0)
            thr = 0x12c000;
        else
            thr = 0x190000;
        thr = thr + 0x64000;
        if (*(int *)(c + 0x464) > thr)
            *(int *)(c + 0x458) = 4;
        else
            *(int *)(c + 0x168) = 0;
    }

    /* cylinder 0 (id @ +0x3a4, flags @ +0x3a0) */
    if (*(u32 *)(c + 0x3a4) != 0) {
        actor = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x3a4));
        if (actor != 0) {
            type = *(u16 *)((char *)actor + 0xc);
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                flags = *(u32 *)(c + 0x3a0);
                if (flags & 0x10) {
                    _ZN6Player16IncMegaKillCountEv(actor);
                    func_02012694(0x1d, c + 0x74);
                    func_ov084_0212ebb4(c);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6f9) != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6fb) == 0) {
                    pos1.x = *(int *)(c + 0x5c);
                    pos1.y = *(int *)(c + 0x60);
                    pos1.z = *(int *)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos1, 3, 0xc000, 1, 0, 1);
                    return;
                }
                if ((flags & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(c);
                return;
            }
            if ((*(u32 *)(c + 0x3a0) & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
            }
        }
    }

    /* cylinder 1 (id @ +0x3d8, flags @ +0x3d4; mega bit read from +0x3a0 per ROM) */
    if (*(u32 *)(c + 0x3d8) != 0) {
        actor = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x3d8));
        if (actor != 0) {
            type = *(u16 *)((char *)actor + 0xc);
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                if ((*(u32 *)(c + 0x3a0) & 0x10) != 0) {
                    _ZN6Player16IncMegaKillCountEv(actor);
                    func_02012694(0x1d, c + 0x74);
                    func_ov084_0212ebb4(c);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6f9) != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
                if ((*(u32 *)(c + 0x3d4) & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(c);
                return;
            }
            if ((*(u32 *)(c + 0x3d4) & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
            }
        }
    }

    /* cylinder 2 (id @ +0x40c, flags @ +0x408) — player only */
    if (*(u32 *)(c + 0x40c) == 0)
        return;
    actor = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x40c));
    if (actor == 0)
        return;
    type = *(u16 *)((char *)actor + 0xc);
    isPlayer = (int)(type == 0xbf);
    if (isPlayer == 0)
        return;
    if ((*(u32 *)(c + 0x408) & 0x10) != 0) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, c + 0x74);
        func_ov084_0212ebb4(c);
        return;
    }
    if (*(u8 *)((char *)actor + 0x6f9) != 0)
        return;
    pos2.x = *(int *)(c + 0x5c);
    pos2.y = *(int *)(c + 0x60);
    pos2.z = *(int *)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos2, 3, 0xc000, 1, 0, 1);
}
