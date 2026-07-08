extern unsigned char NUM_PLAYERS;
extern signed char NUM_RED_COINS;
extern signed char data_0209f30d;

signed char NumRedCoins(void)
{
    int i = 1;
    signed char sum = NUM_RED_COINS;
    signed char *p;
    if (i >= (int)NUM_PLAYERS) return sum;
    p = &data_0209f30d;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)NUM_PLAYERS);
    return sum;
}
