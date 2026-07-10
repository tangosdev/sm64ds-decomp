typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

enum Bool { FALSE, TRUE };

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN5Actor18HorzAngleToCPlayerEv(void *self);
extern int AngleDiff(int a, int b);
extern void _ZN6Player16IncMegaKillCountEv(void *p);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *m, void *f, int a, int b, int fix, u16 t);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int j);
extern void func_02012694(int a, void *p);
extern void func_ov066_02119454(void *c, void *p);

extern void *data_ov066_0211ae5c[];
extern void *data_ov066_0211ae3c[];
extern void *data_ov066_0211ae84[];
extern void *data_ov066_0211aebc[];
extern u8 data_ov066_0211ae08;
extern u8 data_ov066_0211abe0;
extern int data_ov066_0211abe4;
extern int data_ov066_0211b07c;

int func_ov066_0211603c(char *self)
{
    char *actor;
    int flags;
    int hit;
    unsigned int id;
    u16 type;
    enum Bool is_player;

    id = *(unsigned int *)(self + 0x344);
    if (id == 0)
        goto fail;

    actor = (char *)_ZN5Actor10FindWithIDEj(id);
    if (actor == 0)
        return 0;

    if (AngleDiff(*(s16 *)(self + 0x8e), _ZN5Actor18HorzAngleToCPlayerEv(self)) >= 0x4000)
        return 0;

    type = *(u16 *)(actor + 0xc);
    hit = 0;
    flags = *(int *)(self + 0x340);
    is_player = (enum Bool)(type == 0xbf);
    if (is_player == FALSE)
        goto other;

    if (*(u8 *)(actor + 0x6f9) == 1) {
        (*(s8 *)(((int)self + 0x4d8) & 0xFFFFFFFFFFFFFFFF))--;
        hit = 1;
    }
    if (flags & 0x10) {
        (*(s8 *)(((int)self + 0x4d8) & 0xFFFFFFFFFFFFFFFF))--;
        if (*(s8 *)(self + 0x4d8) <= 0)
            _ZN6Player16IncMegaKillCountEv(actor);
        hit = 1;
    }

other:
    if (hit == 0) {
        if (flags & 0x427e0) {
            if (flags & 0x40) {
                if (*(int *)(actor + 8) == 2)
                    (*(s8 *)(((int)self + 0x4d8) & 0xFFFFFFFFFFFFFFFF))--;
            }
            if (flags & 0x40000)
                *(s16 *)(self + 0x4d4) = 1;
            (*(s8 *)(((int)self + 0x4d8) & 0xFFFFFFFFFFFFFFFF))--;
            hit = 1;
        }
    }

    if (hit == 0)
        goto fail;

    if (*(s8 *)(self + 0x4d8) > 0) {
        if (*(int *)(self + 0x49c) == 2) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                self + 0x360, data_ov066_0211ae5c[1], 4, 0x40000000, 0x1000, 0);
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
                self + 0x448, data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
        } else {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                self + 0x360, data_ov066_0211ae84[1], 4, 0x40000000, 0x1000, 0);
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
                self + 0x448, data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
        }
        func_02012694(0x141, self + 0x74);
        return 1;
    }

    {
        u8 x = data_ov066_0211ae08;
        u8 y = data_ov066_0211abe0;
        int side = *(int *)(self + 0x49c);
        data_ov066_0211abe0 = (u8)(y ^ side);
        data_ov066_0211ae08 = (u8)(x + 1);
        data_ov066_0211abe4 = -3;
    }
    func_02012694(0x142, self + 0x74);
    func_ov066_02119454(self, &data_ov066_0211b07c);
    return 2;

fail:
    return 0;
}
