extern short NUM_COINS[];
extern int PLAYER_ARR[];
extern void func_ov002_020b18f0(int a);

void GiveCoins(int idx, int amount)
{
    short *p = NUM_COINS;
    short cur = p[idx];
    short v = cur + amount;
    p[idx] = v;
    v = p[idx];
    if (v > 0xff) {
        p[idx] = 0xff;
    } else if (v < 0) {
        p[idx] = 0;
    }
    func_ov002_020b18f0(PLAYER_ARR[idx]);
}
