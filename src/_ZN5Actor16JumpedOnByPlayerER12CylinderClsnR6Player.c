typedef int s32;
typedef unsigned char u8;

int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(char *self, char *clsn, char *player)
{
    volatile s32 pad[4];
    s32 vel = *(s32 *)(player + 0xa8);

    (void)&pad;
    if (*(s32 *)(clsn + 0x18) & 6) {
        if (vel < 0 && *(u8 *)(player + 0x6de) != 0
            && *(s32 *)(self + 0x60) < *(s32 *)(player + 0x60) - 0xa000)
            return 1;
        return 0;
    } else {
        s32 c10 = *(s32 *)(clsn + 0x10);
        if (vel < 0 && c10 < 0 && c10 > vel)
            return 1;
        return 0;
    }
}
