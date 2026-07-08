struct Actor;
extern unsigned char CURRENT_GAMEMODE;
extern unsigned char data_0208a0e0;
extern void *PLAYER_ARR[];
extern int *_ZN5Actor13ClosestPlayerEv(void *self);

int func_02005e28(unsigned char *self)
{
    int b;
    int i;
    int n;
    b = (int)(CURRENT_GAMEMODE == 1);
    if (b != 0) {
        n = data_0208a0e0;
        for (i = 0; i < n; i++) {
            int *p = (int*)PLAYER_ARR[i];
            if (p != 0) {
                if (p[2] == self[0x113]) return 1;
            }
        }
        return 0;
    } else {
        int *p = _ZN5Actor13ClosestPlayerEv(self);
        int r1;
        if (p != 0) r1 = p[2];
        else r1 = self[0x113];
        return (r1 == self[0x113]) ? 1 : 0;
    }
}
