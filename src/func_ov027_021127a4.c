typedef unsigned char u8;

typedef struct Vector3 {
    int x;
    int y;
    int z;
} Vector3;

extern u8 data_0209f2d8[];

extern int func_ov027_02112618(char *c);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *self, void *actor, int b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *self, void *actor,
                                                            unsigned int msg,
                                                            const Vector3 *pos,
                                                            unsigned int a,
                                                            unsigned int b);
extern int _ZN6Player12GetTalkStateEv(void *self);
extern int _ZN6Player18HasFinishedTalkingEv(void *self);
extern int func_ov027_021124e4(void *slot, void *player);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(int handle, unsigned int a,
                                             unsigned int b, void *pos,
                                             unsigned int c);
extern void func_ov027_02112480(char *slot);

int func_ov027_021127a4(char *c)
{
    int b;
    char *slotBase;
    u8 *idx;
    char *base;
    Vector3 pos;
    int i;
    char *slot;
    int zero;

    b = data_0209f2d8[0];
    b = (b == 2);
    if (b) {
        return 1;
    }

    if (*(u8 *)(c + 0x13d0) == 0) {
        switch (*(u8 *)(c + 0x13d2)) {
        case 0:
            if (func_ov027_02112618(c) == 0) {
                break;
            }
            if (_ZN6Player9StartTalkER9ActorBaseb(*(void **)(c + 0x13c4), c, 1) == 0) {
                break;
            }
            {
                u8 *state = (u8 *)(((int)c + 0x13d2) & 0xFFFFFFFFFFFFFFFF);
                *state = *state + 1;
            }
            break;
        case 1:
            pos.x = *(int *)(c + 0x5c);
            zero = 0;
            pos.y = *(int *)(c + 0x60);
            pos.z = *(int *)(c + 0x64);
            pos.y = pos.y + 0x12c000;
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
                    *(void **)(c + 0x13c4), c, 0xbb, &pos, zero, zero) == 0) {
                break;
            }
            {
                u8 *state = (u8 *)(((int)c + 0x13d2) & 0xFFFFFFFFFFFFFFFF);
                *state = *state + 1;
            }
            break;
        case 2:
            if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x13c4)) == -1) {
                _ZN6Player18HasFinishedTalkingEv(*(void **)(c + 0x13c4));
                *(u8 *)(c + 0x13d0) = 1;
            }
            break;
        }
    } else {
        if (func_ov027_02112618(c) != 0) {
            int *timer = (int *)(((int)c + 0x13c8) & 0xFFFFFFFFFFFFFFFF);
            *timer = *timer + 1;
            base = c + 0x1000;
            if ((*(int *)(base + 0x3c8) & 7) != 0) {
                slotBase = c + 0xd4;
                idx = (u8 *)(c + 0x13d3);
                do {
                    slot = slotBase + *(u8 *)(base + 0x3d3) * 0x60;
                    b = func_ov027_021124e4(slot, *(void **)(base + 0x3c4));
                    *idx = *idx + 1;
                    *idx = *idx % 0x32;
                } while (b == 0);
            }
            *(int *)(c + 0x13cc) =
                _ZN5Sound8PlayLongEjjjRK7Vector3j(*(int *)(c + 0x13cc), 3, 0x184,
                                                  c + 0x74, 0);
        }
    }

    slot = c + 0xd4;
    i = 0;
    do {
        func_ov027_02112480(slot);
        i++;
        slot += 0x60;
    } while (i < 0x32);

    return 1;
}
